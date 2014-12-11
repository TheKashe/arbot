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
#include "GeneticGenome.h"
#include "IGenome.h"

#define POPULATION_SIZE		 10

template <class G>
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
			G genome;
			genome.genomeId=genomeId;
			genome.seedGenome();
		}
	}
	
	static void listPopulation()
	{
		for(byte genomeId=0;genomeId<Population::getPopulationSize();genomeId++)
		{
			G genome;
			genome.genomeId=genomeId;
			char genomeListing[200];
			genome.listGenome(genomeListing,sizeof(genomeListing));
			STDOUTLN(genomeListing);
		}
	}
	
	static void listPopulationSorted(){
		G genomes[POPULATION_SIZE];
		for(byte genomeId=0;genomeId<Population::getPopulationSize();genomeId++)
		{
			genomes[genomeId].genomeId=genomeId;
		}
		qsort(&genomes[0], POPULATION_SIZE, sizeof(G), genome_cmp_desc);
		for(byte genomeId=0;genomeId<Population::getPopulationSize();genomeId++)
		{
			char genomeListing[200];
			genomes[genomeId].listGenome(genomeListing,sizeof(genomeListing));
			STDOUTLN(genomeListing);
		}
	}
	
	
	static uint32_t evolve(){
		DEBUG_STDOUT("###REPOPULATING###");
		
		//sort by success
		G genomes[POPULATION_SIZE];
		for(byte genomeId=0;genomeId<Population::getPopulationSize();genomeId++)
		{
			genomes[genomeId].genomeId=genomeId;
		}
		qsort(&genomes[0], POPULATION_SIZE, sizeof(G), genome_cmp_desc);
#ifdef DEBUG
		DEBUG_STDOUT("sorted\n");
		DEBUG_STDOUT("genome rankings:\n");
		listPopulationSorted();
#endif
		//this is the value we return to the caller
		//not really related to evolution, but we don't want to sort the array twice, so..
		uint32_t mediumTime=-(genomes[POPULATION_SIZE/2].getSuccess());//success is negative, time is positive
		
		for(byte i=0;i<POPULATION_SIZE/2;i++)
		{
			//let's randomly pair two of the best
			//note that this can result in self-couplation
			G mom=genomes[random(0,POPULATION_SIZE/2)];
			G dad=genomes[random(0,POPULATION_SIZE/2)];
			G kid=genomes[POPULATION_SIZE-1-i];   //worst half of the genomes are replaced with new candidates
			kid.reset();
			//always use mom as anchor
			kid.setGeneration(mom.getGeneration()+1);
			kid.setReproductionType(mom.getReproductionType());
			
			switch(kid.getReproductionType()){
				/*case REPRODUCTION_MUTATION:
					kid.reproduceWithMutation(&mom, &dad, &kid);
					break;
				case REPRODUCTION_1BY1CROSSOVER:
					reproduceGeneByGeneCrossover(&mom, &dad, &kid);
					break;
				case REPRODUCTION_POINTCROSSOVER:
					reproduceRandomPointCrossover(&mom, &dad, &kid);*/
				default:
					kid.reproduceWithMutation(&mom, &dad, &kid);
					break;
			}
			
#ifdef DEBUG
			char genomeListing[200];
			mom.listGenome(genomeListing,sizeof(genomeListing));
			DEBUG_STDOUT("\nMOM: ");
			DEBUG_STDOUT(genomeListing);
			dad.listGenome(genomeListing,sizeof(genomeListing));
			DEBUG_STDOUT("\nDAD: ");
			DEBUG_STDOUT(genomeListing);
			kid.listGenome(genomeListing,sizeof(genomeListing));
			DEBUG_STDOUT("\nKID: ");
			DEBUG_STDOUT(genomeListing);
			DEBUG_STDOUT("\n");
#endif
		}
#ifdef DEBUG
		DEBUG_STDOUT("\n\nNew generation:\n");
		listPopulation();
#endif
		return mediumTime;
	}
	
};


#endif
