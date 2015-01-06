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
    Population(string dataFilePath); //Constructor
    void Evolve( ); //Evolution 
    void PrintEnd( ); //Final output 
    virtual ~Population( ); //Destructor
    friend ostream & operator<<(ostream & os, const Population &source); //overloaded '<<' operator
    int suffix_cntr; //File suffix
private:
    //Self-identifying variables.
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

    Helper h; //Random number generating helper
    Chromosome **individuals; //Population array
    TimeSlot ** timeSlots; //timeslot array
    
    int ** incompatibleSections; //list of incompatible section for each section
    int ** creditTimeSlot;//list of timeslot for each credit
    int ** sectionProf; //list of professor for each section
    int ** profSection; //list of section for each professor
    int ** courseSection; //list of section for each course
    int ** sectionPref; //list of preferences for each section
    int ** profPref; //list of preferences for each prof
    double * timeCredLegend; //credit value of each timeslot
    double * sectionCredit; //credit value of each section
    double * profCreditMax; //max credits for each professor

    ofstream statFile;
    ofstream outputFile;
    ofstream debug;

    void readDatFiles( ); //Read the input data file
    void readSectionList(ifstream &input, vector<string> &); //Read section list
    void readCourseList(ifstream &input, vector<string>); //Read the course list
    void readProfessorList(ifstream &input, vector<string>); //Read the professor list
    void readTimeSlotList(ifstream &input); //Read the timeslot list
    void readInitialSchedule(ifstream &input); //Read the initial schedule
    void initPopulationFromFirst( ); //Create the initial population from the initial schedule
    void readCoursePref(ifstream &input, vector<string>); //Read the course time preferences
    void readProfPref(ifstream &input); //Read professor time preferences
    void readParameters(ifstream &input); //Read parameters for the program
    void prepareDataStatistics( ); //Prepare data statistics about input
    int getWeightedRandomIndividual( ); //Get weighted random individual
    string PrintTableFormat( );
    string GetFitnessData( ); //Get a string showing the fitness data of the population
    bool allValid( ); //Check if all individuals are valid
    bool allValid(int currentGeneration );
};

#endif
