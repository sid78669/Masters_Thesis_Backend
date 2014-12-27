#ifndef CHROMOSOME_H
#define CHROMOSOME_H  

#define DEBUG_REPAIR 0
#define DEBUG_FITNESS 0
#define DEBUG_ASSIGNMENT 0
#define DEBUG_MUTATION 0
#define DELTA_MAX -2.0
#define DELTA_MIN 2.0
#define MAX_FITNESS 100000

#include "Utility.h"
#include "Gene.h"
#include "Helper.h"
#include "TimeSlot.h"
#include "PeriodData.h"
#include <iostream>
#include <iomanip>
#include <math.h>
#include <algorithm>

using namespace std;
class Population;
class Helper;

class Chromosome {
public:
    Chromosome(int geneLength, int profCount, double * profCredsMax);
    Chromosome(const Chromosome *);
    Chromosome(const Chromosome &);
    virtual ~Chromosome( );
    Chromosome& operator=(const Chromosome &source);

    void setTime(int geneID, int newTime);
    void setProf(int geneID, int newProf, double courseCred);
    void setGene(int geneID, Gene);
    //void setProfCredit(int * profCreds);
    double getCourseLoad(int profID);
    const int getTime(int geneID);
    const int getProf(int geneID);
    Gene* getGene(int geneID);

    int getFitness( );

    string print( );
    string getProfessorLoads( );
    string print(int gene);
    string printTable(TimeSlot ** timeSlots, int timeslot_count);
    string printTable(TimeSlot ** timeSlots, int timeslot_count, double * sectionCredit);
    string printProfTable( );

    void mutate(int ** sectionProf, int section_count, int ** creditTimeSlot, TimeSlot ** timeSlots, double * timeCredLegend, int timeCredLegendSize, Helper * h, double mutation_probability, double * sectionCredit);
    void repair(int ** sectionProf, int section_count, int ** creditTimeSlot, TimeSlot ** timeSlots, double * timeCredLegend, int timeCredLegendSize, Helper * h, int ** incompatibleSections, const int REPAIR_TRIES, double * sectionCredit, int ** profSection);
    void updateFitness(int ** incompatibleSections, int ** sectionPref, int ** profPref, TimeSlot ** timeSlots, int timeslot_count, int ** profSectionsTaught);

    friend bool operator==(Chromosome &ch1, Chromosome &ch2);

private:
    Gene ** genes;
    double * professorCredits;
    double * professorCreditsInitial;
    int gene_length;
    int prof_count;
    int fitness;
    void updateProfLoad(double * sectionCredit);
    bool validProfessorLoad(int profID);
    bool validProfessorLoadChange(int profID, double creditChange);
};

#endif
