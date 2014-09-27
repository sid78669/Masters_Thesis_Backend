#ifndef CHROMOSOME_H
#define CHROMOSOME_H  

#include "Gene.h"

using namespace std;

class Chromosome {
public:
    Chromosome(int geneLength);
    Chromosome(Chromosome *&);
    ~Chromosome( );

    void setTime(int geneID, int newTime);
    void setProf(int geneID, int newProf);
    void setGene(int geneID, Gene);

    int getTime(int geneID);
    int getProf(int geneID);
    Gene getGene(int geneID);

    void setFitness(int);
    int getFitness( );

    string print( );
    string print(int gene);
private:
    Gene *genes;
    int gene_length;
    int fitness;

};

#endif