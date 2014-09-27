#include "Chromosome.h"


Chromosome::Chromosome(int geneLength) {
    gene_length = geneLength;
    fitness = 0;
    genes = new Gene[geneLength];
}

Chromosome::Chromosome(Chromosome *& other) {
    gene_length = other->gene_length;
    genes = new Gene[gene_length];
    fitness = other->fitness;
    for (int i = 0; i < gene_length; i++) {
        setGene(i, other->getGene(i));
    }
}


Chromosome::~Chromosome( ) {
    delete[] genes;
}

void Chromosome::setTime(int geneID, int newTime) {
    genes[geneID].setTimeID(newTime);
}

void Chromosome::setProf(int geneID, int newProf) {
    genes[geneID].setProfID(newProf);
}

void Chromosome::setGene(int geneID, Gene newGene) {
    genes[geneID].setProfID(newGene.getProfID( ));
    genes[geneID].setTimeID(newGene.getTimeID( ));
}

int Chromosome::getTime(int geneID) {
    return genes[geneID].getTimeID( );
}

int Chromosome::getProf(int geneID) {
    return genes[geneID].getProfID( );
}

Gene Chromosome::getGene(int geneID) {
    return genes[geneID];
}

int Chromosome::getFitness( ) {
    return fitness;
}

void Chromosome::setFitness(int newFitness ) {
    fitness = newFitness;
}

string Chromosome::print( ) {
    string rtnVal;
    for (int i = 0; i < gene_length; i++) {
        rtnVal += to_string(i) + "> " + to_string(genes[i].getProfID( )) + ", " + to_string(genes[i].getTimeID( )) + "\n";
    }

    rtnVal += "----------------------------\n";

    return rtnVal;
}

string Chromosome::print(int geneID) {
    return to_string(genes[geneID].getProfID( )) + ", " + to_string(genes[geneID].getTimeID( ));
}