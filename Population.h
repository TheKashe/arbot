//
//  Population.h
//
//
//  Created by Jernej Kaše on 12/6/14.
//
//

#ifndef _Population_h
#define _Population_h

#include "Genome.h"

#define POPULATION_SIZE		 10

class Population{
public:
	static byte getPopulationSize()
	{
		return POPULATION_SIZE;
	}
	
	static void seedPopulation()
	{
		arduinoRandomize();
		//
		for(byte genomeId=0;genomeId<Population::getPopulationSize();genomeId++)
		{
			Genome genome;
			genome.genomeId=genomeId;
			genome.seedGenome();
		}
	}
	
	static void listPopulation()
	{
		for(byte genomeId=0;genomeId<Population::getPopulationSize();genomeId++)
		{
			Genome genome;
			genome.genomeId=genomeId;
			char genomeListing[200];
			genome.listGenome(genomeListing,sizeof(genomeListing));
			STDOUTLN(genomeListing);
		}
		/*
		qsort(genomes, Population::getPopulationSize(), sizeof(struct genome), genome_cmp_desc);
		Serial.println(" ");
		for(byte i=0; i<Population::getPopulationSize();i++)
		{
			Serial.print("genome:");
			Serial.print(genomes[i].genome);
			Serial.print(", health:");
			Serial.print(genomes[i].health);
			Serial.print(", success:");
			Serial.println(genomes[i].success);
		}*/
	}
	
	static uint32_t evolve(){
		DEBUG_STDOUT("###REPOPULATING###");
		
		//sort by success
		Genome genomes[POPULATION_SIZE];
		for(byte genomeId=0; genomeId<POPULATION_SIZE;genomeId++)
		{
			genomes[genomeId].genomeId=genomeId;
		}
		qsort(&genomes[0], POPULATION_SIZE, sizeof(Genome), genome_cmp_desc);
		DEBUG_STDOUT("sorted\n");
		
		uint32_t mediumTime=-(genomes[POPULATION_SIZE/2].getSuccess());//success is negative, time is positive
		
		int j=POPULATION_SIZE-1;
		for(byte i=0;i<=POPULATION_SIZE/2;i++)
		{
			Genome mom=genomes[i];
			Genome kid=genomes[j];   //worst in the list order, not in eprom order
			
			kid.reset();
			kid.setGeneration(mom.getGeneration()+1);
			
			for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++)
			{
				byte gene=mom.getGene(geneId);
				
				// 1/10 chance of mutation
				byte dice=random(0,11);
				if(dice==7){
					DEBUG_STDOUT("MUTATING");
					DEBUG_STDOUT((int)gene);
					DEBUG_STDOUT("-->");
					dice=random(0,8);
					gene ^= 1 << dice; //this should randomly flip a single bit
					DEBUG_STDOUT((int)gene);
					DEBUG_STDOUT("\n");
				}
				kid.setGene(geneId,gene);
			}
			j--;
			//old way
			//we randomly crossower dad's and mom's genes
			/*if(random(0,2)){
					Serial.println("using mom's command and dad's time");
					gene=(m_gene & G_MASK) | (d_gene & T_MASK);
			}
			else {
				Serial.println("using dad's command and mom's time");
				gene=(d_gene & G_MASK) |( m_gene & T_MASK);
			}*/
			//make mutants 5% chance as we are kindof low on time
		}
		return mediumTime;
	}
	
};


#endif
