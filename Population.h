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
	}
	
	static void listPopulationSorted(){
		Genome genomes[POPULATION_SIZE];
		for(byte genomeId=0;genomeId<Population::getPopulationSize();genomeId++)
		{
			genomes[genomeId].genomeId=genomeId;
		}
		qsort(&genomes[0], POPULATION_SIZE, sizeof(Genome), genome_cmp_desc);
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
		Genome genomes[POPULATION_SIZE];
		for(byte genomeId=0;genomeId<Population::getPopulationSize();genomeId++)
		{
			genomes[genomeId].genomeId=genomeId;
		}
		qsort(&genomes[0], POPULATION_SIZE, sizeof(Genome), genome_cmp_desc);
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
			Genome mom=genomes[random(0,POPULATION_SIZE/2)];
			Genome dad=genomes[random(0,POPULATION_SIZE/2)];
			Genome kid=genomes[POPULATION_SIZE-1-i];   //worst half of the genomes are replaced with new candidates
			kid.reset();
			//always use mom as anchor
			kid.setGeneration(mom.getGeneration()+1);
			kid.setReproductionType(mom.getReproductionType());
			
			switch(kid.getReproductionType()){
				case REPRODUCTION_MUTATION:
					reproduceWithMutation(&mom, &dad, &kid);
					break;
				case REPRODUCTION_1BY1CROSSOVER:
					reproduceGeneByGeneCrossover(&mom, &dad, &kid);
					break;
				case REPRODUCTION_POINTCROSSOVER:
					reproduceRandomPointCrossover(&mom, &dad, &kid);
				default:
					reproduceWithMutation(&mom, &dad, &kid);
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
private:
	static byte mutateGene(byte gene, const byte probability)
	{
		if(probability==0)
			return gene;
		byte dice=random(0,100/probability);
		if(dice==0){
#ifdef DEBUG
			byte command=gene & G_MASK;
			byte time=gene & T_MASK;
			DEBUG_STDOUT("MUTATING");
			DEBUG_STDOUT((int)command);
			DEBUG_STDOUT(",");
			DEBUG_STDOUT((int)time);
			DEBUG_STDOUT("-->");
#endif
			dice=random(0,8);
			gene ^= 1 << dice; //this should randomly flip a single bit
#ifdef DEBUG
			command=gene & G_MASK;
			time=gene & T_MASK;
			DEBUG_STDOUT((int)command);
			DEBUG_STDOUT(",");
			DEBUG_STDOUT((int)time);
			DEBUG_STDOUT("\n");
#endif
		}
		return gene;
	}
	
	/* duplicat mom's genome with random mutations.
	 * dad's genome is ignored
	 */
	static void reproduceWithMutation(const Genome *mom,
									  const Genome *dad,
									  Genome *kid)
	{
		DEBUG_STDOUT("\nReproducint with 10% mutation\n");
		for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++)
		{
			byte gene=mom->getGene(geneId);
			kid->setGene(geneId,mutateGene(gene,10));
		}
		
	}
	
	/* randomly pick mom's or dad's gene
	 */
	static void reproduceGeneByGeneCrossover(const Genome *mom,
											 const Genome *dad,
											 Genome *kid)
	{
		DEBUG_STDOUT("\nReproducint with gene by gene crossover and 5% mutation\n");
		for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++)
		{
			byte m_gene=mom->getGene(geneId);
			byte d_gene=dad->getGene(geneId);
			byte k_gene;
			if(random(0,2)){
				k_gene=(m_gene & G_MASK) | (d_gene & T_MASK);
			}
			else {
				k_gene=(d_gene & G_MASK) |( m_gene & T_MASK);
			}
			kid->setGene(geneId,mutateGene(k_gene,5));
		}
		
	}
	
	/* randomly pick mom's or dad's gene
	 */
	static void reproduceRandomPointCrossover(const Genome *mom,
											 const Genome *dad,
											 Genome *kid)
	{
		DEBUG_STDOUT("\nReproducint with gene by gene crossover and 5% mutation\n");
		
		byte crossoverPoint=random(0,STALL_GENE_INDEX);
		for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++)
		{
			byte m_gene=mom->getGene(geneId);
			byte d_gene=dad->getGene(geneId);
			byte k_gene;
			if(geneId<crossoverPoint){
				k_gene=m_gene;			}
			else {
				k_gene=d_gene;
			}
			kid->setGene(geneId,mutateGene(k_gene,5));
		}
		
	}
	
};


#endif
