//
//  FreedomController.h
//
//
//  Created by Jernej Kase on 11/21/14.
//
//  The objective of this controller is to find freedom.
//  Freedom is defined as 5 seconds of continous motion. The robot will try to find freedom, after finding freedom it will stop
//  If it finds itself in helpless situation, it will give up


#ifndef ____FreedomController__
#define ____FreedomController__

#include "ArbotController.h"
#include "IArbotVMA03MDPlatform.h"
#include "GeneticGenome.h"
#include "Population.h"
#include "NeuralNetwork.h"

#define DONT_MOVE               false //true only for debug when we don't want robot to move
									  //#define SIM_REPOPULATE false

//states
#define STARTING			0
#define NEXT_SEQUENCE		1
#define NEXT_LOOP			2
#define PINGING				3
#define	DONE_PING			4
#define THINKING			5
#define MOVING				6
#define STALLED				7
#define DONE_MOVE			8
#define SEQUENCE_TIMEOUT	9
#define SEQUENCE_DONE		10
#define GENERATION_DONE		11
#define SLEEP				12
#define FREE				255

//some constants for eeprom locations
#define EEPROM_CURRENT_GENOMEID_ADDR     1023
#define EEPROM_CURRENT_GENERATIONID_ADDR 1021   // int  - 2 bytes
#define EEPROM_GENERATION_BEST_TIME      1017   // long - 4 bytes
#define EEPROM_SEQUENCE_TIMEOUT          1013   // long - 4 bytes
#define EEPROM_GENERATION_FREE_CNT       1012   // byte


class FreedomController:
public ArbotController{
private:
	uint16_t			moveTimer;				//move timeout in ms
	uint16_t			timeMultiplier;			//by how much we multiply time in gene command
	uint16_t			moveTimeout;			//is the absolute move timeout, regardless of timer
	uint32_t			startedMovingTimestamp;
	uint32_t			sequenceStartTimestamp;
	uint16_t			pingbackTime;
	byte				currentGenomeId;
	GeneticGenome*		currentGenome;
	byte				currentStep;
	boolean				interruptsDisabled;		//disable "interrupts" for bumping while in interrupt handler
	byte				lastCommand;
	byte				lastTime;
	NeuralNetwork		neuralNet;
	IArbotVMA03MDPlatform *arbotVma03MdPlatform;
protected:
	byte          state;
public:
	FreedomController(	IArbotVMA03MDPlatform	*arbotPlatform,
						const uint16_t			timeMultiplier=150): //move timeout, in ms
	ArbotController(arbotPlatform)
	{
		this->timeMultiplier	= timeMultiplier;
		arbotVma03MdPlatform	= arbotPlatform;
		currentGenomeId			= 0;
		currentStep				= 0;
		currentGenome			= NULL;
		moveTimer				= 0;
		interruptsDisabled		= false;
		lastCommand				= 0;
		lastTime				= 0;
		state					= STARTING;
		moveTimeout				= (uint16_t)(((float)timeMultiplier*33.0)*1.2);
		
		
		//TODO
		
		float weights[6]={/*l2n1*/0.525157,/*l2n2*/0.804601,/*l3n1*/0.183411,0.014265,/*l3n2*/0.387660,0.371998};
		neuralNet=NeuralNetwork();
		neuralNet.setWeights(weights);
		
		
		arduinoRandomize();
	}
	
	
	/* must be called or robot won't work
	 *  and we MUST call base class setup to setup the platform, too
	 */
	virtual void setup()
	{
		ArbotController::setup();
		delay(5000);  //operator friendly delay before we start going
	}
	
	//use the existing genome, but restart all stats and begin with defaults
	void reset(uint32_t defaultSequenceTimeout=1800000)
	{
		saveCurrentGenomeId(0); //when wee seed genome, we also set currentGenome to 0
		saveCurrentGenerationId(0);
		saveSequenceTimeout(defaultSequenceTimeout);
		uint32_t tmp=2147483647; //worst possible long time
		EEPROM_writeAnything(EEPROM_GENERATION_BEST_TIME,tmp);
		EEPROM.write(EEPROM_GENERATION_FREE_CNT,0);
	}
	
	
	/*/	this is the "main" method where we react based on what state we are in
	 *	all methods should only change state and never invoke other methods
	 *
	 *	Sequence	- is a genome used until goal is reached or sequence times out
	 *
	 * Loop		- is a repetition of one genome first to last gene
	 *				  when we reach the last gene, we repeat the whole loop again
	 *				  until either sequence times out or we reach the goal
	 *
	 *				  Essentially, loop is series of thinks and moves
	 *				  which can be interrupted by stalls
	 *
	/*/
	virtual void loop()
	{
		Serial.print("current state:");
		Serial.println(state);
		switch(state){
			case STARTING:
				state=NEXT_SEQUENCE;
				break;
			case NEXT_SEQUENCE:
				initSequence();
				break;
			case NEXT_LOOP:
				initLoop();
				break;
			case DONE_MOVE:
				ping();
				break;
			case PINGING:
				checkPingback();
				break;
			case DONE_PING:
				think();
				break;
			case THINKING:
				this->move(this->lastCommand,this->lastTime);
				break;
			case MOVING:
				checkFreedom();
				break;
			case STALLED:
				interruptsDisabled?setStateDoneMove():stallInterrupt();
				break;
			case FREE:
				finishSequence();
				break;
			case SEQUENCE_TIMEOUT:
				finishSequence();
				break;
			case SEQUENCE_DONE:
				switchGenome();
				break;
			case GENERATION_DONE:
				initNextGeneration();
				break;
			case SLEEP:
				arbotPlatform->blink(10000);
				break;
		}
	}

/*-----------------------------------------------------------------------------------------------------------*/
private:
	void initSequence()
	{
		Serial.println("initSequence");
		if(currentGenome!=NULL)
			delete currentGenome;
		currentGenomeId	= getCurrentGenomeId();
		currentGenome	= new GeneticGenome(currentGenomeId);
		currentStep		= 0;
		state			= NEXT_LOOP;
		arbotVma03MdPlatform->init();
		
		char diag[200];
		snprintf(diag,sizeof(diag), "DIAG;genome;");
		currentGenome->listGenome(&diag[strlen(diag)],sizeof(diag)-strlen(diag)-1);
		arbotVma03MdPlatform->showDiagnostics(diag);
		
		delay(1000);
		sequenceStartTimestamp=millis();
	}
	
	
	void initLoop()
	{
		//go back to first gene
		currentStep	= 0;
		state		= DONE_MOVE;
		
		//if we timed out the method below sets timeout state
		if(checkForSequenceTimeout())return; //return is just in case we add somethign below
	}
	
	void ping(){
		arbotPlatform->fullStop();			//just to be sure
		arbotVma03MdPlatform->ping();			//just to be sure
		state=PINGING;
	}
	
	void checkPingback(){
		pingbackTime=arbotVma03MdPlatform->getPingReply();
		if(pingbackTime==PING_WAITING){
			state=PINGING;
			return;
		}
		state=DONE_PING;
	}
	
	void think()
	{
		arbotPlatform->fullStop();			//just to be sure
		
		state				= THINKING;
		interruptsDisabled	= false;
		
		float logPingback=(float)log10(pingbackTime);
		
		float inputs[]={logPingback};
		float outputs[2];
		neuralNet.reset();
		neuralNet.setInputs(inputs,1);
		neuralNet.calculate();
		neuralNet.getOutputs(&outputs[0],2);
		
		//gene is now output of neural net
		byte gene=(byte)outputs[0];
		gene=gene<<5;
		gene=gene | (((byte)outputs[1]) & 0b00000111);
		geneToCommand(gene);
	}
	
	
	void stallInterrupt()
	{
		arbotPlatform->fullStop();
		blink(1);
		
		state				= THINKING;					//stall interrupt is a form of thinking in special conditions
		interruptsDisabled	= true;
		geneToCommand(currentGenome->getStallGene());	//this will continue in move...
	}
	
	
	void move(byte command, byte time)
	{
		state = MOVING;
		if(DONT_MOVE) command=G_NOP;			//prevent moving... useful for debugging only
		switch(command){
			case G_FORWARDS:
				arbotPlatform->forwards(100);
				break;
			case G_BACKWARDS:
				arbotPlatform->backwards(100);
				break;
			case G_LEFT:
				arbotPlatform->left(100);
				break;
			case G_RIGHT:
				arbotPlatform->right(100);
				break;
			case G_NOP:
				arbotPlatform->fullStop();
				break; //mkey...?
		}
		if((time & 0b00011111)==0b00011111) {
			moveTimer=0;
		}
		else {
			//in the command we have only 5 bits for time, so we multiply it by factor
			//which can be different e.g. for sim or robot
			moveTimer=(time+1)*timeMultiplier;
		}
		startedMovingTimestamp=millis();
		delay(200); //we need some delay for the robot to start moving
	}
	
	
	void checkFreedom()
	{
		delay(50); //don't check every clock cycle
		if(arbotVma03MdPlatform->isFree()){ //only sensor is good indicator of freedom
			state = FREE;
			return;
		}
		
		if(!arbotVma03MdPlatform->isMoving()){
			state = STALLED;
			return;
		}
		
		uint32_t timer = millis()-startedMovingTimestamp; //freedom is moving straight without bumping into anything
		if ((timer>=moveTimeout) || (moveTimer!=0 && timer>=moveTimer)){ //did we exceed move comand timer or are we spinning in circles?
			state = DONE_MOVE; //this will stop and move to next step
			return;
		}
		if(checkForSequenceTimeout()) return; //return is just in case we add somethign below
	}
	
	
	byte checkForSequenceTimeout()
	{
		uint32_t sequenceDuration=millis()-sequenceStartTimestamp;
		
		char diag[100];
		snprintf(diag, sizeof(diag), "DIAG;timer;time(ms):%u", sequenceDuration);
		arbotVma03MdPlatform->showDiagnostics(diag);
		
		if(sequenceDuration>=getSequenceTimeout()){ //robot who doesn't find his way in 30' is a dead robot
			state = SEQUENCE_TIMEOUT;
			return 1;
		}
		return 0;
	}
	
	
	/*/ Finish sequence handles two states!
	 *	 FREE					- means the goal is achieved
	 *	 SEQUENCE_TIMEOU	- robot failed
	/*/
	void finishSequence()
	{
		int32_t sequenceDuration = millis()-sequenceStartTimestamp;
		int32_t award;
		
		arbotPlatform->fullStop();
		arbotVma03MdPlatform->terminate();

		if(state==FREE){
			award=-sequenceDuration; //shorter is better, so let's just mirror it over negative axis
			EEPROM.write(EEPROM_GENERATION_FREE_CNT,EEPROM.read(EEPROM_GENERATION_FREE_CNT)+1);
		}
		else if(state==SEQUENCE_TIMEOUT){
			award= -2147483648; //least possible award
		}
		currentGenome->setSuccess(award);
		
		int32_t bestTime;
		EEPROM_readAnything(EEPROM_GENERATION_BEST_TIME,bestTime);
		if(sequenceDuration<bestTime && state==FREE){
			EEPROM_writeAnything(EEPROM_GENERATION_BEST_TIME,sequenceDuration);
			
			//just output some diagnostics
			char diag[100];
			snprintf(diag,sizeof(diag), "DIAG;besttime;best time (ms):%u", sequenceDuration);
			arbotVma03MdPlatform->showDiagnostics(diag);
		}
		
		state = SEQUENCE_DONE;
	}
	
	
	void switchGenome()
	{
		Serial.print("switch genome, new genome id:");
		//switching genome
		currentGenomeId++;
		
		if(currentGenomeId>=Population<GeneticGenome>::getPopulationSize()){
			state=GENERATION_DONE;
			return;
		}
		Serial.println(currentGenomeId);
		saveCurrentGenomeId(currentGenomeId);
		setStateAfterGenomeSwitch();
	}
	
	
	/*/ We need this to have different implementation for simulator */
	virtual void setStateAfterGenomeSwitch()
	{
		state = SLEEP; //note, we will need hard reset to restart robot
	}
	
	
	//Just to be consistant and use states
	void setStateDoneMove()
	{
		state = DONE_MOVE;
	}
	
	
	//command and time should only be set here to be consistent
	//we use them as parameter to move and as
	//memory to check what that command was
	void geneToCommand(byte gene)
	{
		if((gene & G_MASK)==G_REVERT){
			switch(lastCommand){
				case G_FORWARDS:
					lastCommand=G_BACKWARDS;
					break;
				case G_BACKWARDS:
					lastCommand=G_FORWARDS;
					break;
				case G_LEFT:
					lastCommand=G_RIGHT;
					break;
				case G_RIGHT:
					lastCommand=G_LEFT;
					break;
				default:
					lastCommand=G_NOP;
					break;
			}
		}
		else{
			lastCommand=gene & G_MASK;
		}
		lastTime=gene & T_MASK;
	}
	
	
	//TODO this should go to platform
	void blink(byte count)
	{
		for(byte i=0;i<count;i++){
			digitalWrite(13, HIGH);
			delay(100);
			digitalWrite(13, LOW);
		}
	}
	
	void initNextGeneration()
	{
		//show current generation diag messages
		byte  genFreeCnt = EEPROM.read(EEPROM_GENERATION_FREE_CNT);
		int32_t bestTime;
		EEPROM_readAnything(EEPROM_GENERATION_BEST_TIME,bestTime);
		
		char diag[100];
		snprintf(diag,sizeof(diag), "DIAG;gentimes;<br/> generation:%i, best:%i, free count:%i", getCurrentGenerationId(),bestTime,genFreeCnt);
		arbotVma03MdPlatform->showDiagnostics(diag);

		//pick winners and make copies
		uint32_t mediumTime = Population<GeneticGenome>::evolve();
			
		//init next generation
		saveCurrentGenomeId(0);     //after repopulation, we start with first genome, we also set currentGenome to 0
		saveCurrentGenerationId(getCurrentGenerationId()+1);  //increment generation counter
		EEPROM.write(EEPROM_GENERATION_FREE_CNT,0);
		
		uint32_t genTimout;
		EEPROM_readAnything(EEPROM_SEQUENCE_TIMEOUT, genTimout);
		if(genFreeCnt>=Population<GeneticGenome>::getPopulationSize()/2 && ((float)mediumTime*1.1)<((float)genTimout)){
			genTimout=(int32_t)((float)mediumTime*1.1);
			saveSequenceTimeout(genTimout);
		}
		
		//wait for oper or autoreset in sim
		setStateAfterGenomeSwitch();
	}
	
/*-----------------------------------------------------------------------------------------
		HELPERS
*/

	//id is simply the sequence number of the genome
	void saveCurrentGenomeId(byte currentGenomeId)
	{
		EEPROM.write(EEPROM_CURRENT_GENOMEID_ADDR,currentGenomeId);
	}
	
	byte getCurrentGenomeId()
	{
		return EEPROM.read(EEPROM_CURRENT_GENOMEID_ADDR);
	}
	
	//id is simply the sequence number of the genome
	void saveCurrentGenerationId(uint16_t currentGenerationId)
	{
		EEPROM_writeAnything(EEPROM_CURRENT_GENERATIONID_ADDR,currentGenerationId);
		
		char diag[100];
		snprintf(diag,sizeof(diag), "DIAG;generation;current generation: %i", currentGenerationId);
		arbotVma03MdPlatform->showDiagnostics(diag);
	}
	
	uint16_t getCurrentGenerationId()
	{
		uint16_t currentGenerationId;
		EEPROM_readAnything(EEPROM_CURRENT_GENERATIONID_ADDR,currentGenerationId);
		
		char diag[100];
		snprintf(diag,sizeof(diag), "DIAG;generation;current generation: %i", currentGenerationId);
		arbotVma03MdPlatform->showDiagnostics(diag);
		
		return currentGenerationId;
	}
	
	uint32_t getSequenceTimeout()
	{
		uint32_t timeout;
		EEPROM_readAnything(EEPROM_SEQUENCE_TIMEOUT,timeout);
		
		char diag[100];
		snprintf(diag, sizeof(diag), "DIAG;timeout;seq timeout(ms): %u", timeout);
		arbotVma03MdPlatform->showDiagnostics(diag);

		return timeout;
	}
	
	void saveSequenceTimeout(uint32_t timeout)
	{
		EEPROM_writeAnything(EEPROM_SEQUENCE_TIMEOUT,timeout);
		
		char diag[100];
		snprintf(diag, sizeof(diag), "DIAG;timeout;seq timeout(ms): %u", timeout);
		arbotVma03MdPlatform->showDiagnostics(diag);
	}
	
};


#endif /* defined(____FreedomController__) */

