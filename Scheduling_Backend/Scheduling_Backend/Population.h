#ifndef POPULATION_H
#define POPULATION_H

#define ARRAY_MAX 5000
#define DEBUG_SECTION 0
#define DEBUG_COURSES 0
#define DEBUG_PROF 0
#define DEBUG_TIMESLOT 0
#define DEBUG_INIT_CHROMOSOME 1

#include "Chromosome.h"
#include "TimeSlot.h"
#include "Utility.h"
#include "Helper.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Population {
public:
    Population(string dataFilePath, int populationSize, int generationCount, double mutationProbability);
    ~Population( );
private:
    string data_file_path;
    int population_size;
    int generation_count;
    double mutation_probability;
    int section_count;
    int courseCount;
    int professorCount;
    int timeSlotCount;
    int creditCount;
    Chromosome **individuals;
    TimeSlot ** timeSlots;
    int ** incompatibleSections;
    int ** creditTimeSlot;
    int ** sectionProf;
    int ** courseSection;
    double * sectionCredit;


    void readDatFiles( );
    void readSectionList(vector<string> &);
    void readCourseList(vector<string> );
    void readProfessorList(vector<string> ); 
    void readTimeSlotList( );
    void readInitialSchedule( );
    void initPopulationFromFirst( );

    Chromosome * mutate(int individualID);

};

#endif