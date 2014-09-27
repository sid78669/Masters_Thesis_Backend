#ifndef TIMECHROMOSOME_H
#define TIMECHROMOSOME_H

#include "TimeGene.h"
#include "Chromosome.h"

class TimeChromosome {
public:
    TimeChromosome(int geneLength, Chromosome source, int timeGeneLength);
    ~TimeChromosome( );

    void setSection(int geneID, int newSection);
    void setProf(int geneID, int newProf);
    void setGene(int geneID, TimeGene);

    int getSection(int geneID);
    int getProf(int geneID);
    TimeGene getGene(int geneID);


};


#endif