//
//  Genome.h
//  
//
//  Created by Jernej Kaše on 12/6/14.
//
//

#ifndef _Genome_h
#define _Genome_h

#include "IGenome.h"


//genome structure, bytes:
//b0-bx		- genes of type T
//b16-17    - generation,			uint16
//b18-21    - success,				uint32
//b22		- reproduction type,	byte

#define GENERATION_GENE_INDEX	(geneSize*length)
#define SUCCESS_GENE_INDEX		(GENERATION_GENE_INDEX+2)		// generation is 2 bytes
#define	REPRODUCTION_TYPE_INDEX	(SUCCESS_GENE_INDEX + 4)		// success is 4 bytes
#define	GENOME_LENGTH			(REPRODUCTION_TYPE_INDEX+1)		// reproduction type is 1 byte


#define OFFSET (genomeId*GENOME_LENGTH)

#define REPRODUCTION_MUTATION		0
#define REPRODUCTION_1BY1CROSSOVER	1
#define REPRODUCTION_POINTCROSSOVER	2

template <class T>
class Genome:public virtual IGenome{
private:
public:
	byte genomeId;
	byte geneSize;		//size of a single gene, 1-4 bytes
	byte length;
	
	Genome()
	{
		geneSize=sizeof(T);
		length=6;			//for NN, this is the number of weights
	}
	
	explicit Genome(byte genomeId)
	{
		this->genomeId=genomeId;
		success=getSuccess();
		geneSize=sizeof(T);
		length=6;			//for NN, this is the number of weights
	}
	
	virtual ~Genome()
	{
	}
	
	void getAllGenes(T genes[])
	{
		for(byte geneId=0;geneId<length;geneId++)
		{
			genes[geneId]=getGene(geneId);
		}
	}
	
	virtual byte getLength() const
	{
		return length;
	}

	
	int32_t getSuccess(){
		int32_t success=0;
		EEPROM_readAnything(OFFSET+SUCCESS_GENE_INDEX, success);
		this->success=success;
		return success;
	}
	
	virtual void setSuccess(int32_t success)
	{
		this->success=success;
		EEPROM_writeAnything(OFFSET+SUCCESS_GENE_INDEX,success);
	}
	
	T getGene(byte geneId) const
	{
		//no clue what to do if gene is out of bounds??
		if(geneId>=length) return 0;
		T value;
		EEPROM_readAnything(OFFSET+(geneId*sizeof(T)), value);
		return value;
	}
	
	void setGene(byte geneId, T value)
	{
		EEPROM_writeAnything(OFFSET+(geneId*sizeof(T)),value);
	}
	
	byte getReproductionType()
	{
		uint8_t reproduction;
		EEPROM_readAnything(OFFSET+REPRODUCTION_TYPE_INDEX, reproduction);
		return reproduction;
	}
	
	
	void setReproductionType(byte reproductionType)
	{
		EEPROM_writeAnything(OFFSET+REPRODUCTION_TYPE_INDEX,reproductionType);
	}

	
	uint16_t getGeneration()
	{
		uint16_t generation;
		EEPROM_readAnything(OFFSET+GENERATION_GENE_INDEX, generation);
		return generation;
	}
	
	void setGeneration(uint16_t generation)
	{
		EEPROM_writeAnything(OFFSET+GENERATION_GENE_INDEX,generation);
	}
	
	virtual void reset()
	{
		T zero=0;
		for(byte geneId=0;geneId<length;geneId++)
		{
			setGene(geneId,zero);
		}
		//b16-17    - generation
		setGeneration(0);
		
		//b18-21    - success, worst possible success
		setSuccess(-2147483647);
	}
	
	
	//only for float really..
	virtual void seedGenome()
	{
		reset(); //zero out and set initial values
		
		for(byte geneId=0;geneId<length;geneId++)
		{
			//x=random number between -5 and 5
			float x = (((float)rand()/(float)(RAND_MAX)) * 10)-5;
			//T dice = random(0,1);
			setGene(geneId,x);
		}
		
		//set random reproduction type
		//byte dice=random(0,3);
		byte dice=0; //only mutation for now
		setReproductionType(dice);
		
	}
	
	virtual void listGenome(char* outStr, size_t strLen)
	{
		snprintf(outStr,strLen, "genome:%i,gen:%i,succ:%d,rep:%i::",
				 genomeId,getGeneration(),getSuccess(),getReproductionType());
		
		for(byte geneId=0;geneId<length;geneId++)
		{
			T gene=getGene(geneId);
			snprintf(outStr,strLen,"%s%f,",outStr,gene);
		}
		
	}
	

	
	/* duplicat mom's genome with random mutations.
	 * dad's genome is ignored
	 */
	static void reproduceWithMutation(const Genome *mom,
									  const Genome *dad,
									  Genome *kid)
	{
		DEBUG_STDOUT("\nReproducint with 10% mutation\n");
		for(byte geneId=0;geneId<mom->getLength();geneId++)
		{
			T gene=mom->getGene(geneId);
			kid->setGene(geneId,mutateGene(gene,10));
		}
		
	}
	
	
	/* randomly pick mom's or dad's gene
	 */
	/*static void reproduceRandomPointCrossover(const Genome *mom,
									   const Genome *dad,
											 Genome *kid)
	{
		DEBUG_STDOUT("\nReproducint with gene by gene crossover and 5% mutation\n");
		
		byte crossoverPoint=random(0,mom->getLength());
		for(byte geneId=0;geneId<mom->getLength();geneId++)
		{
			T m_gene=mom->getGene(geneId);
			T d_gene=dad->getGene(geneId);
			T k_gene;
			if(geneId<crossoverPoint){
				k_gene=m_gene;			}
			else {
				k_gene=d_gene;
			}
			kid->setGene(geneId,mutateGene(k_gene,5));
		}
		
	}
	
	
	virtual void reproduceGeneByGeneCrossover(const Genome *mom,
											 const Genome *dad,
											 Genome *kid)
	{
		DEBUG_STDOUT("\nReproducint with gene by gene crossover and 5% mutation\n");
		for(byte geneId=0;geneId<=STALL_GENE_INDEX;geneId++)
		{
			T m_gene=mom->getGene(geneId);
			T d_gene=dad->getGene(geneId);
			T  k_gene;
			if(random(0,2)){
				k_gene=(m_gene & G_MASK) | (d_gene & T_MASK);
			}
			else {
				k_gene=(d_gene & G_MASK) |( m_gene & T_MASK);
			}
			kid->setGene(geneId,mutateGene(k_gene,5));
		}
		
	}*/
	
	static T mutateGene(T gene, const byte probability)
	{
		if(probability==0)
			return gene;
		byte dice=random(0,100/probability);
		if(dice==0){
/*#ifdef DEBUG
			byte command=gene & G_MASK;
			byte time=gene & T_MASK;
			DEBUG_STDOUT("MUTATING");
			DEBUG_STDOUT((int)command);
			DEBUG_STDOUT(",");
			DEBUG_STDOUT((int)time);
			DEBUG_STDOUT("-->");
#endif*/
			dice=random(0,sizeof(T)/sizeof(byte)); //this should work with any data type, incluing float
			byte* b=reinterpret_cast<byte*>(&gene);
			*(b+dice) ^= 1 << random(0,7) ; //this should randomly flip a single bit
/*#ifdef DEBUG
			command=gene & G_MASK;
			time=gene & T_MASK;
			DEBUG_STDOUT((int)command);
			DEBUG_STDOUT(",");
			DEBUG_STDOUT((int)time);
			DEBUG_STDOUT("\n");
#endif*/
		}
		return gene;
	}
	
};

//compare genomes for sort
//for some reason comparing on method results fails
//but we can use the metods to load the value to success, so...
int genome_cmp_desc(const void *a, const void *b)
{
	IGenome *ia = (IGenome*)a;
	IGenome *ib = (IGenome*)b;
	ib->getSuccess();
	ia->getSuccess();
	//we could overflow int here...
	if(ib->success>ia->success) return 1;
	if(ib->success==ia->success) return 0;
	if(ib->success<ia->success) return -1;
	return 0; //just to be sure
}

#endif
