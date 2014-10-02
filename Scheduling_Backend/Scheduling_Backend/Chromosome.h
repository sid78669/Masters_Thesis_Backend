#ifndef CHROMOSOME_H
#define CHROMOSOME_H  

#define DEBUG_REPAIR 0
#define DEBUG_FITNESS 0
#define DEBUG_ASSIGNMENT 1
#define DEBUG_MUTATION 0

#include "Gene.h"
#include "Helper.h"
#include "TimeSlot.h"
#include <iostream>

using namespace std;
class Population;
class Helper;

class Chromosome {
public:
    Chromosome(int geneLength);
    Chromosome(const Chromosome *);
    ~Chromosome( );
    //Chromosome& operator=(const Chromosome &source);

    void setTime(int geneID, int newTime);
    void setProf(int geneID, int newProf);
    void setGene(int geneID, Gene);

    int getTime(int geneID);
    int getProf(int geneID);
    Gene* getGene(int geneID);

    int getFitness( );

    string print( );
    string print(int gene);

    void mutate(int ** sectionProf, int section_count, int ** creditTimeSlot, TimeSlot ** timeSlots, Helper * h, double mutation_probability );
    void repair(int ** sectionProf, int section_count, int ** creditTimeSlot, TimeSlot ** timeSlots, Helper * h, int ** incompatibleSections, const int REPAIR_TRIES);
    void updateFitness(int ** incompatibleSections );

    friend bool operator==(Chromosome &ch1, Chromosome &ch2);
private:
    Gene *genes;
    int gene_length;
    int fitness;

};

#endif