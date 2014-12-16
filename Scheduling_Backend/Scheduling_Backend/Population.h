#ifndef POPULATION_H
#define POPULATION_H

#define ARRAY_MAX 5000
#define DEBUG_SECTION 0
#define DEBUG_COURSES 0
#define DEBUG_PROF 0
#define DEBUG_TIMESLOT 0
#define DEBUG_INIT_CHROMOSOME 0
#define DEBUG_INIT_POPULATION 0
#define DEBUG_INIT_POPULATION_COMPARED 0
#define DEBUG_VALIDATE 0
#define DEBUG_EVOLVE 0
#define DEBUG_LO 0
#define DEBUG_INIT 0
#define DEBUG_PRINTTABLE 0
#define DEBUG_PROF_NEW 0
#define PRINT_WHOLE_POPULATION 0

#include "Chromosome.h"
#include "TimeSlot.h"
#include "Utility.h"
#include "Helper.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Chromosome;

class Population {
public:
    //Population(string dataFilePath, int populationSize, int generationCount, int replacementWait, double mutationProbability);
    Population(string dataFilePath);
    void Evolve( );
    string PrintTableFormat( );
    string GetFitnessData( );
    virtual ~Population( );
    friend ostream & operator<<(ostream & os, const Population &source);
private:
    string data_file_path;
    double mutation_probability; 
    double initFitness;
    int population_size;
    int generation_count;
    int replacement_wait;
    int section_count;
    int course_count;
    int professor_count;
    int timeslot_count;
    int credit_count;
    int weakestIndividualID;
    int strongestIndividualID;
    int lowestFitnessSeen;
    int highestFitnessSeen;
    const int REPAIR_TRIES;
    Helper h;
    Chromosome **individuals;
    TimeSlot ** timeSlots;
    double * timeCredLegend;
    int ** incompatibleSections;
    int ** creditTimeSlot;
    int ** sectionProf;
    int ** courseSection;
    int ** sectionPref;
    int ** profPref;
    double * sectionCredit;
    bool * individualValidity;
    int * profSectionsTaught;
    double * profCreditMax;

    void readDatFiles( );
    void readSectionList(ifstream &input, vector<string> &);
    void readCourseList(ifstream &input, vector<string>);
    void readProfessorList(ifstream &input, vector<string>);
    void readTimeSlotList(ifstream &input);
    void readInitialSchedule(ifstream &input);
    void initPopulationFromFirst( );
    void readCoursePref(ifstream &input, vector<string>);
    void readProfPref(ifstream &input);
    void readParameters(ifstream &input);
    bool validateChromosome(Chromosome * const toValidate) const;
    string validatePrint(int i);

    int localOptimization(int currentGeneration );
};

#endif
