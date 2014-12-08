//
//  Genome.h
//  
//
//  Created by Jernej Kaše on 12/6/14.
//
//

#ifndef _Genome_h
#define _Genome_h

//genome structure, bytes:
//b0-b14	- commands
//b15		- command for bump(stall) interrupt
//b16-17    - generation
//b18-21    - success

#define GENOME_LENGTH			22
#define GENOME_LAST_CMD_INDEX	14
#define STALL_GENE_INDEX		15
#define GENERATION_GENE_INDEX	16
#define SUCCESS_GENE_INDEX		18

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

#define OFFSET (genomeId*GENOME_LENGTH)

class Genome{
private:
public:
	int32_t success;
	byte genomeId;
	
	Genome()
	{
	}
	
	explicit Genome(byte genomeId)
	{
		this->genomeId=genomeId;
		success=getSuccess();
	}
	
	virtual ~Genome()
	{
	}
	
	int32_t getSuccess(){
		int32_t success=0;
		EEPROM_readAnything(OFFSET+SUCCESS_GENE_INDEX, success);
		this->success=success;
		return success;
	}
	
	void setSuccess(int32_t success)
	{
		this->success=success;
		setGeneEx(SUCCESS_GENE_INDEX,success);
	}
	
	byte getGene(byte geneId)
	{
		//no clue what to do if gene is out of bounds??
		if(geneId>STALL_GENE_INDEX) return 0;
		return EEPROM.read(OFFSET+geneId);
	}
	
	byte getStallGene()
	{
		return getGene(STALL_GENE_INDEX);
	}
	
	void setGene(byte geneId, byte value)
	{
		EEPROM.write(OFFSET+geneId,value);
	}

	
	uint16_t getGeneration()
	{
		uint16_t generation=0;
		EEPROM_readAnything(OFFSET+GENERATION_GENE_INDEX, generation);
		return generation;
	}
	
	void setGeneration(uint16_t generation)
	{
		setGeneEx(OFFSET+GENERATION_GENE_INDEX,generation);
	}
	
	void reset(){
		for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++)
		{
			setGene(geneId,0);
		}
		//b16-17    - generation
		setGeneration(0);
		
		//b18-21    - success, worst possible success
		setSuccess(-2147483647);
	}
	
	/*/ Generates random genome */
	void seedGenome()
	{
		Serial.print("seeding genome:");
		Serial.println(genomeId);
		
		reset(); //zero out and set initial values
		
		for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++)
		{
			byte command;
			byte speed;
			
			//throw the dice for command
			int dice=1;
			while(dice==0 || dice==1 || dice==7){ //commands 0b001 and 0b111 is not implemented at the moment and let's ignore NOP
				dice = random(0,8);
			}
			command	= dice<<5; //simply shift dice by 5 to get to the command
			
			//throw dice foc time
			dice	= random(0,2);
			if(dice==0 && !(command==G_LEFT ||command==G_RIGHT)){ //don't allow unlimited circling
				speed = TIL_HIT;
			}
			else {
				speed = random(0,31);
			}
			setGene(geneId,command | speed);
		}
	}
	
	void listGenome(char* outStr, size_t strLen)
	{
		snprintf(outStr,strLen, "genome:%i,gen:%i,succ:%d::",genomeId,getGeneration(),getSuccess());
		
		for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++) //we start at interupt command
		{
			byte gene=getGene(geneId);
			byte command=gene & G_MASK;
			byte time=gene & T_MASK;
			
			const char* cmd;
			switch(command){
				case G_FORWARDS:
					cmd="fwd";
					break;
				case G_BACKWARDS:
					cmd="bck";
					break;
				case G_LEFT:
					cmd="lft";
					break;
				case G_RIGHT:
					cmd="rgh";
					break;
				case G_NOP:
					cmd="nop";
					break;
				case G_REVERT:
					cmd="rev";
					break;
					//case 5: command=PAUSE;break; //maybe in the future
			}
			snprintf(outStr,strLen,"%s%s:%d,",outStr,cmd,time);
		}
	}
	
private:
	template <class T> void setGeneEx(int geneId, const T& value)
	{
		EEPROM_writeAnything(OFFSET+geneId, value);
	}
		
};

//compare genomes for sort
//for some reason comparing on method results fails
//but we can use the metods to load the value to success, so...
int genome_cmp_desc(const void *a, const void *b)
{
	Genome *ia = (Genome*)a;
	Genome *ib = (Genome*)b;
	ib->getSuccess();
	ia->getSuccess();
	//we could overflow int here...
	if(ib->success>ia->success) return 1;
	if(ib->success==ia->success) return 0;
	if(ib->success<ia->success) return -1;
	return 0; //just to be sure
}

#endif
