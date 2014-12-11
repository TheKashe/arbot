//
//  Genome.h
//
//
//  Created by Jernej Kaše on 12/6/14.
//
//

#ifndef _GeneticGenome_h
#define _GeneticGenome_h

#include "Genome.h"

//genome structure, bytes:
//b0-b14	- commands
//b15		- command for bump(stall) interrupt
//b16-17    - generation
//b18-21    - success
//b22		- reproduction type


#define GENOME_LAST_CMD_INDEX	4								// by changing this number we actually controll number of genes in genome
#define STALL_GENE_INDEX		(GENOME_LAST_CMD_INDEX + 1)

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

#define REPRODUCTION_MUTATION		0
#define REPRODUCTION_1BY1CROSSOVER	1
#define REPRODUCTION_POINTCROSSOVER	2

class GeneticGenome:public Genome<byte>{
private:
public:
	
	GeneticGenome():Genome()
	{
	}
	
	explicit GeneticGenome(byte genomeId):Genome(genomeId)
	{
		this->genomeId=genomeId;
		success=getSuccess();
	}
	
	virtual ~GeneticGenome()
	{
	}
	
	
	byte getStallGene()
	{
		return getGene(STALL_GENE_INDEX);
	}
	
	
	virtual void reset()
	{
		byte zero=0;
		for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++)
		{
			setGene(geneId,zero);
		}
		//b16-17    - generation
		setGeneration(0);
		
		//b18-21    - success, worst possible success
		setSuccess(-2147483647);
	}
	
	/*/ Generates random genome */
	virtual void seedGenome()
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
		
		//set random reproduction type
		byte dice=random(0,3);
		setReproductionType(dice);
	}
	
	
	virtual void listGenome(char* outStr, size_t strLen)
	{
		snprintf(outStr,strLen, "genome:%i,gen:%i,succ:%d,rep:%i::",
				 genomeId,getGeneration(),getSuccess(),getReproductionType());
		
		for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++)
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
				default:
					cmd="nop";
					break;
			}
			snprintf(outStr,strLen,"%s%s:%d,",outStr,cmd,time);
		}
	}
		
};

#endif
