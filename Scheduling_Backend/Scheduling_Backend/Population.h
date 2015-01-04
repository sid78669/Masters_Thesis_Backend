/*
Author: Siddharth Dahiya
Package: Course and Professor Scheduling (Backend)
File: Population.h
Contact: syd5144@gmail.com

Copyright (c) 2015 Siddharth Dahiya

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

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
#define DEBUG_EVOLVE 0
#define DEBUG_LO 0
#define DEBUG_INIT 0
#define DEBUG_PRINTTABLE 0
#define DEBUG_PROF_NEW 0
#define PRINT_WHOLE_POPULATION 0

#include "Chromosome.h"
#include "TimeSlot.h"
#include "Utility.cpp"
#include "Helper.cpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
using namespace std;

class Chromosome;

class Population {
public:
    //Population(string dataFilePath, int populationSize, int generationCount, int replacementWait, double mutationProbability);
    Population(string dataFilePath);
    void Evolve( );
    void PrintEnd( );
    virtual ~Population( );
    friend ostream & operator<<(ostream & os, const Population &source);
    int suffix_cntr;
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
    
    int ** incompatibleSections;
    int ** creditTimeSlot;
    int ** sectionProf;
    int ** profSection;
    int ** courseSection;
    int ** sectionPref;
    int ** profPref;
    double * timeCredLegend;
    double * sectionCredit;
    double * profCreditMax;

    ofstream statFile;
    ofstream outputFile;
    ofstream debug;

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
    void prepareDataStatistics( );
    int getWeightedRandomIndividual( );
    string PrintTableFormat( );
    string GetFitnessData( );
    bool allValid( );
    bool allValid(int currentGeneration );
};

#endif
