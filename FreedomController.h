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
#include <EEPROM.h>
#include "randomseed.h"

#define FREEDOM    10000 //freedom is defined as uninterrupted STRAIGHT FORWARD movement
#define FREEDOM_SENSOR_PIN  7
#define FREEDOM_INTERRUPT   4
#define LOOP_TIMEOUT 10  //this should be shorter and shorter with each generation.. no point in replicating losing genomes

#define SEED_GENOME    false //set to true reseed genome... otherwise whatever is in EPROM is used
#define SIM_REPOPULATE false
#define DONT_MOVE      false //true only for debug
//genes
//first 3 bytes are commands
//last 5 bytes are duration
#define G_NOP       0b00000000 // no operation
//                  0b00100000
#define G_LEFT      0b01000000
#define G_RIGHT     0b01100000
#define G_FORWARDS  0b10000000
#define G_BACKWARDS 0b10100000
#define G_REVERT    0b11000000
//#define G_PAUSE     0b11100000 //not implemented

#define G_MASK      0b11100000
#define T_MASK      0b00011111
//define subcommands / speed
#define TIL_HIT     0b00011111  //move until hit
#define TIMER_STEP  200         //how much does 1 timer command tick mean for moves

#define GENOME_LENGTH  20
#define POPULATION_SIZE 10
//genome structure, bytes:
  //b0                 = generation
  //b1                 = health
  //b2                 = success
  //b3                 = command for bump interriupt
  //b4-bGENOME_LENGTH  = algorythm
#define GENOME_START 4

//states
#define STARTING      0
#define NEXT_SEQUENCE 1
#define NEXT_LOOP     2
#define THINKING      3
#define MOVING        4
#define STALLED       5
#define DONE_MOVE     6
#define REPOPULATING  7
#define SLEEP         8
#define FREE          255

//some constants for eeprom locations
#define EEPROM_CURRENT_GENOMEID_ADDR     1023
#define EEPROM_CURRENT_GENERATIONID_ADDR 1022

//helper for compare, must be outside the class
struct genome { 
  byte genome;
  byte health;
  byte success;
};

//compare genomes for sort
int genome_cmp_desc(const void *a, const void *b) 
{ 
  struct genome *ia = (struct genome *)a;
  struct genome *ib = (struct genome *)b;
  return (int)(ib->success - ia->success);
} 

int genome_cmp_asc(const void *a, const void *b) 
{ 
  struct genome *ia = (struct genome *)a;
  struct genome *ib = (struct genome *)b;
  return (int)(ia->success-ib->success);
} 

boolean freedom; //global variable as we can't use class member function

void freedomInterrupt()
{
  Serial.println("freedom interrupt!");
  freedom=true;
  detachInterrupt(FREEDOM_INTERRUPT); //we reset before next run anyway
}

class FreedomController:
public ArbotController{
  byte state;
  byte timedFlag;
  byte straightMoveFlag; //only straight FORWARD movement qualifies for freedom
  unsigned long startedMovingTimestamp;
  unsigned long lastBumpTimestamp;
  byte currentGenome;
  byte currentSequence;
  unsigned long sequenceStartTimestamp;
  byte currentStep;
  boolean interruptsDisabled; //disable interrupts while in interrupt handler
  byte lastCommand;
  byte lastTime;
  byte bumpCount;
  ArbotVMA03MDPlatform *arbotVma03MdPlatform;
public:
  FreedomController(ArbotVMA03MDPlatform *arbotPlatform):
  ArbotController(arbotPlatform){
    arbotVma03MdPlatform=arbotPlatform;
    state=STARTING;
    int seed=seedOut(31);
    Serial.print("seed");
    Serial.println(seed);
    randomSeed(seed);
    currentGenome=0;
    currentStep=GENOME_START;
    timedFlag=0;
    interruptsDisabled=false;
    lastCommand=0;
    lastTime=0;
    bumpCount=0;
    currentSequence=0;
  }

  // initialises genome in eprom
  //genome structure, bytes:
  //b0                 = generation
  //b1                 = health
  //b2                 = success
  //b3                 = command for bump interriupt
  //b4-bGENOME_LENGTH  = algorythm
  void seedGenome()
  {
    int seed=seedOut(31);
    Serial.print("seed");
    Serial.println(seed);
    randomSeed(seed);
    //
    for(byte i=0;i<POPULATION_SIZE;i++)
    {
      Serial.print("seeding genome:");
      Serial.println(i);
      int offset=i*GENOME_LENGTH;
      //b0, generation
      EEPROM.write(offset+0,0);
      Serial.print(0);
      //b1, health
      EEPROM.write(offset+1,255);
      Serial.print(255);
      //b2, success
      EEPROM.write(offset+2,0);
      Serial.print(0);
      Serial.print(":");
      //b3 stall interrupt - we generate stall interrupt randomly in this process
      //random algorythim
      for(byte j=3;j<GENOME_LENGTH;j++)
      {
        byte command;
        byte speed;
        //throw the dice for command
        int dice=1;
        while(dice==0 || dice==1 || dice==7){ //commands 0b001 and 0b111 is not implemented at the moment and let's ignore NOP
          dice = random(0,8); 
        }
        // Serial.print("int command:");
        //Serial.println(dice);
        //Serial.println(dice,BIN);
        command=dice<<5; //simply shift dice by 5 to get to the command
        //Serial.println(command,BIN);
        //moving till we hit sth or time based move?
        dice= random(0,2);
        if(dice==0 && !(command==G_LEFT ||command==G_RIGHT)){ //don't allow unlimited circling
          speed=TIL_HIT;
          Serial.print("NOLIM");
        } 
        else {
          speed=random(0,31);
          Serial.print(speed);
        }
        if(i==0 and j==3){ //let's seed REVERSE interrupt handler for this one genome, just to make sure we have one
          Serial.print("OVERRIDE");
          command=G_REVERT;
          speed=5;
        }
        //Serial.print(command | speed,BIN);
        Serial.print(":");
        EEPROM.write(offset+j,command | speed); 
      }
      Serial.println("");
    }
    saveCurrentGenomeId(0); //when wee seed genome, we also set currentGenome to 0
    saveCurrentGenerationId(0);
  }

  void listGenome()
  {
    Serial.print("current Genome Id:");
    Serial.println(getCurrentGenomeId());
    Serial.print("current generation Id:");
    Serial.println(getCurrentGenerationId());
    //
    for(byte i=0;i<POPULATION_SIZE;i++)
    {
      Serial.print("genome:");
      Serial.println(i);
      int offset=i*GENOME_LENGTH;
      //b0, generation;
      Serial.print(EEPROM.read(offset+0));
      Serial.print(",");
      //b1, health
      Serial.print(EEPROM.read(offset+1));
      Serial.print(",");
      //b2, success;
      Serial.print(EEPROM.read(offset+2));
      Serial.print(":");
      //random algorythim
      for(byte j=3;j<GENOME_LENGTH;j++)
      {
        byte gene=EEPROM.read(i*GENOME_LENGTH+j);
        Serial.print(gene,BIN);
        Serial.print("-");
        byte command=gene & G_MASK;
        byte time=gene & T_MASK; 
        switch(command){
        case G_FORWARDS: 
          Serial.print("fwd"); 
          break;
        case G_BACKWARDS: 
          Serial.print("bwd"); 
          break;
        case G_LEFT: 
          Serial.print("lft");
          break;
        case G_RIGHT: 
          Serial.print("rgh");
          break;
        case G_NOP: 
          Serial.print("nop");
          break;
        case G_REVERT:
          Serial.print("revert");
          break;
          //case 5: command=PAUSE;break; //maybe in the future
        }
        if((time & 0b00011111)==0b00011111) { 
          Serial.print("NOLIM");
        } 
        else {
          Serial.print(time);
        }
        Serial.print(":");
      }
      Serial.println("");
    }
    struct genome genomes[POPULATION_SIZE];
    //increase generation counter
    for(byte i=0; i<POPULATION_SIZE;i++)
    {
      int offset=i*GENOME_LENGTH;
      genomes[i].genome=i;
      genomes[i].health=EEPROM.read(offset+1);
      genomes[i].success=EEPROM.read(offset+2);
    }
    qsort(genomes, POPULATION_SIZE, sizeof(struct genome), genome_cmp_desc);
    Serial.println(" ");
    for(byte i=0; i<POPULATION_SIZE;i++)
    {
      Serial.print("genome:");
      Serial.print(genomes[i].genome);
      Serial.print(", health:");
      Serial.print(genomes[i].health);
      Serial.print(", success:");
      Serial.println(genomes[i].success);
    }
  }

  virtual void setup(){
    ArbotController::setup();
    delay(2000);
    if(SEED_GENOME) {
      delay(5000); 
      seedGenome();
    }
    listGenome();
    if(SIM_REPOPULATE){
      while(true){
        repopulate();
        listGenome();
      }
    }
    pinMode(13, OUTPUT);  
    pinMode(FREEDOM_SENSOR_PIN,INPUT_PULLUP);
    attachInterrupt(FREEDOM_INTERRUPT,freedomInterrupt,LOW);
    delay(5000);  //operator friendly delay before we start going
  }
  
  //Just to be consistant and use states 
  void setStateDoneMove()
  {
    state=DONE_MOVE;
  }

  virtual void loop(){
    if(SEED_GENOME) {delay(1000); return;} //don't do processing while seeding
    Serial.print("current state:");
    Serial.println(state);
    switch(state){
    case STARTING: 
      lastBumpTimestamp=millis(); 
      state=NEXT_SEQUENCE; 
      break;
    case NEXT_SEQUENCE:
      initSequence();
      break;
    case NEXT_LOOP:
      initLoop();
      break;
    case MOVING: 
      checkFreedom(); 
      break;
    case DONE_MOVE: 
      think(); 
      break;
    case STALLED: 
      interruptsDisabled?setStateDoneMove():stallInterrupt(); 
      break;
    case FREE:
      finishSequence();
      break;
    case REPOPULATING: 
      repopulate(); 
      break;
    case SLEEP: 
      digitalWrite(13, HIGH);
      delay(10000); 
      break;
    }
  }
  
  void initSequence()
  {
    Serial.println("initSequence");
    currentGenome=getCurrentGenomeId();
    currentStep=GENOME_START;
    bumpCount=0;
    Serial.print("Using next genome:");
    Serial.println(currentGenome);
    delay(1000);
    sequenceStartTimestamp=millis();
    state=NEXT_LOOP;
    freedom=false;
  }
  
  void finishSequence()
  {
    Serial.print("finishSequence in x minutes:");
    arbotPlatform->fullStop();
    //how long dit it take to solve the maze?
    byte sequenceDurationMinutes=(millis()-sequenceStartTimestamp)/60000;
    Serial.println(sequenceDurationMinutes);
    byte award=255-sequenceDurationMinutes; //each minute is one penalty point
    if(sequenceDurationMinutes>255) award=0; //just to be sure
    saveSuccessPoints(award);
    switchGenome();
  }
  
  void switchGenome(){
    Serial.print("switch genome, new genome id:");
    //switching genome
    currentGenome++;
    Serial.println(currentGenome);
    saveCurrentGenomeId(currentGenome);
    
    if(currentGenome>=POPULATION_SIZE-1){
      state=REPOPULATING;
      return;
    }
    state=SLEEP; //note, we will need hard reset to restart
  }
  
  void initLoop()
  {
    Serial.print("initLoop, time from sequence start: ");
    byte sequenceDurationMinutes=(millis()-sequenceStartTimestamp)/60000;
    Serial.println(sequenceDurationMinutes);
    if(sequenceDurationMinutes>=LOOP_TIMEOUT){ //robot who doesn't find his way in 30' is a dead robot
      saveSuccessPoints(0);
      switchGenome();
      return;
    }
    currentStep=GENOME_START;
    state=DONE_MOVE;
  }

  //command and time should only be set here to be consistent
  //we use them as parameter to move and as
  //memory to check what that command was
  void geneToCommand(byte gene){
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

  void blink(byte count){
    for(byte i=0;i<count;i++){
      digitalWrite(13, HIGH); 
      delay(100);
      digitalWrite(13, LOW);
    }
  }

  void stallInterrupt()
  {
    blink(1);
    bumpCount++;
    interruptsDisabled=true;
    arbotPlatform->fullStop(); 
    byte gene=EEPROM.read(currentGenome*GENOME_LENGTH+GENOME_START-1); //this is the command for interrupt
    geneToCommand(gene);
    move(lastCommand,lastTime);
  }

  void think(){
    arbotPlatform->fullStop(); 
    state=THINKING;
    interruptsDisabled=false;

    currentStep++;
    if(currentStep>=GENOME_LENGTH-1) {
      state=NEXT_LOOP;
      return;
    }

    byte gene=EEPROM.read(currentGenome*GENOME_LENGTH+currentStep);
    geneToCommand(gene);
    move(lastCommand,lastTime);
  }

  void move(byte command, byte time){
    state=MOVING;
    if(DONT_MOVE) command=G_NOP; //prevent moving... useful for debugging
    switch(command){
    case G_FORWARDS: 
      straightMoveFlag=1; 
      arbotPlatform->forwards(100);  
      break;
    case G_BACKWARDS: 
      straightMoveFlag=1; 
      arbotPlatform->backwards(100);  
      break;
    case G_LEFT: 
      straightMoveFlag=0; 
      arbotPlatform->left(100);  
      break;
    case G_RIGHT: 
      straightMoveFlag=0; 
      arbotPlatform->right(100); 
      break;
    case G_NOP:
      straightMoveFlag=0;
      arbotPlatform->fullStop(); 
      break; //mkey...?
    }
    if((time & 0b00011111)==0b00011111) { 
      timedFlag=0;
    } 
    else {
      timedFlag=time+1;
    }
    startedMovingTimestamp=millis();
    delay(200); //we need some delay for the robot to start moving
  }

  void checkFreedom(){
    delay(50); //don't check every clock cycle
    unsigned long freedomTimer=millis()-startedMovingTimestamp; //freedom is moving straight without bumping into anything
    if(freedom){ //only sensor is good indicator of freedom
      state=FREE;
    } 
    else if(!arbotVma03MdPlatform->isMoving()){
      //we just bumped :-(
      //just stop to be sure...
      arbotPlatform->fullStop();
      lastBumpTimestamp=millis();
      state=STALLED;
    } 
    else if ((!straightMoveFlag && freedomTimer>=FREEDOM) || (timedFlag!=0 && freedomTimer>=timedFlag*TIMER_STEP)){ //did we exceed move comand timer or are we spinning in circles?
      state=DONE_MOVE; //this will stop and move to next step
    }
  }

  void repopulate(){
    Serial.println("###REPOPULATING###");

    //sort by success
    struct genome genomes[POPULATION_SIZE];
    //increase generation counter
    for(byte i=0; i<POPULATION_SIZE;i++)
    {
      int offset=i*GENOME_LENGTH;
      genomes[i].genome=i;
      genomes[i].health=EEPROM.read(offset+1);
      genomes[i].success=EEPROM.read(offset+2);
    }
    qsort(genomes, POPULATION_SIZE, sizeof(struct genome), genome_cmp_desc);

    Serial.println("sorted");

    //this will rewrite unsucessful genomes with new kids
    int j=POPULATION_SIZE-1;
    for(byte i=0;i<POPULATION_SIZE/2+1;i+=2)
    {
      byte mom=genomes[i].genome;
      byte dad=genomes[i+1].genome;
      byte kid=genomes[j].genome; //worst in the list, not in eprom
      Serial.print("mom:"); 
      Serial.println(mom);
      Serial.print("dad:"); 
      Serial.println(dad);
      Serial.print("kid"); 
      Serial.println(kid);

      byte m_generation=EEPROM.read(mom*GENOME_LENGTH);
      byte d_generation=EEPROM.read(dad*GENOME_LENGTH);
      EEPROM.write(kid*GENOME_LENGTH,(m_generation>d_generation?m_generation:d_generation)+1);
      for(byte k=3; k<GENOME_LENGTH;k++){
        Serial.print("gene "); 
        Serial.print(k);
        byte gene;
        byte m_gene=EEPROM.read(mom*GENOME_LENGTH+k);
        byte d_gene=EEPROM.read(dad*GENOME_LENGTH+k);
        Serial.print("m_gene:");
        Serial.println(m_gene, BIN);
        Serial.print("d_gene:");
        Serial.println(d_gene, BIN);
        //we randomly crossower dad's and mom's genes
        if(random(0,2)){
          Serial.println("using mom's command and dad's time");
          gene=(m_gene & G_MASK) | (d_gene & T_MASK);
        } 
        else {
          Serial.println("using dad's command and mom's time");
          gene=(d_gene & G_MASK) |( m_gene & T_MASK);
        }
        Serial.print("kid's gene:");
        Serial.println(gene, BIN);
        //make mutants 5% chance as we are kindof low on time
        byte dice=random(0,21);
        if(dice==20){
          Serial.println("MUTATING");
          Serial.print(gene);
          Serial.print("-->");
          dice=random(0,8);
          gene ^= 1 << dice; //this should randomly flip a single bit
        }
        Serial.println(gene,BIN);
        EEPROM.write(kid*GENOME_LENGTH+k,gene);
        Serial.println(kid*GENOME_LENGTH+k);
      }
      j--;
    }

    //reset health and success
    for(byte i=0; i<POPULATION_SIZE;i++)
    {
      int offset=i*GENOME_LENGTH;
      EEPROM.write(offset+1,255); //reset health
      EEPROM.write(offset+2,0); //reset success
    }
    saveCurrentGenomeId(0);     //after repopulation, we start with first genome, we also set currentGenome to 0
    saveCurrentGenerationId(getCurrentGenerationId()+1);  //increment generation counter
    state=SLEEP;
  }
  
  void saveSuccessPoints(byte award){
    EEPROM.write(currentGenome*GENOME_LENGTH+2,award);
  }
  
  
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
  void saveCurrentGenerationId(byte currentGenerationId)
  {
    EEPROM.write(EEPROM_CURRENT_GENERATIONID_ADDR,currentGenerationId);
  }
  
  byte getCurrentGenerationId()
  {
    return EEPROM.read(EEPROM_CURRENT_GENERATIONID_ADDR);
  }
  
  

};


#endif /* defined(____FreedomController__) */

