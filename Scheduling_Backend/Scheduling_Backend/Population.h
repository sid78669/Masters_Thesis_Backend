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
    Population(string dataFilePath, string destinationFolder, int suffixCounter, bool _console); //Constructor
    void Evolve( ); //Evolution 
    void PrintEnd( ); //Final output 
    virtual ~Population( ); //Destructor
    int suffix_cntr; //File suffix
private:
    //Self-identifying variables.
    string data_file_path;
    string destination_folder;
    stringstream key;
    const bool console; //If true, output to console.
    //Ascending order of being read from data file
    int generation_count;
    int population_size;
    int replacement_wait;
    int mutation_probability; 
    int section_count;
    int professor_count;
    int timeslot_count;
    int credit_count;
    
    //Predetermined variable
    const int REPAIR_TRIES;

    //Generated variables
    double initFitness;
    int weakestIndividualID;
    int strongestIndividualID;
    int lowestFitnessSeen;
    int highestFitnessSeen;
    

    //Inorder of being read or generated
    bool ** incompatibleSectionsMatrix; //Adjacency matrix for incompatible sections.
    double * sectionCredit; //credit value of each section
    double * profCreditMax; //max credits for each professor
    int ** sectionProf; //list of professor for each section
    int ** profSection; //list of section for each professor
    int ** sectionTimeslot; //list of timeslots for each section
    int ** sectionPref; //list of preferences for each section
    int ** profPref; //list of preferences for each prof
    int *** associatedProfessors; //list of professors that are associated by a course.
    double * timeCredLegend; //legend for creditTimeSlot Array
    int ** creditTimeSlot;//list of timeslot for each credit

    double * timeCredits;
    bool ** timeslotDaytime;
    bool ** timeslotConflict;
    bool ** timeslotConsecutive;
    bool ** timeslotSpread;

    int * sortedSectionList;
    bool sortByProfessorCount(int i, int j);
    bool sortByTimeslotCount(int i, int j);
    
    Chromosome **individuals; //Population array

    Helper h; //Random number generating helper

    ofstream statFile;
    ofstream outputFile;
    ofstream debug;

    void readDatFiles( ); //Read the input data file
    void readParameters(ifstream &input); //Read parameters for the program
    void readSectionList(ifstream &input); //Read section list
    void readSectionCreditList(ifstream &input); //Read section list
    void readProfessorCreditList(ifstream &input); //Read section list
    void readSectionProfessorList(ifstream &input); //Read section prof list
    void readSectionTimeslotList(ifstream &input); //Read section timeslot list
    void readProfessorSectionList(ifstream &input); //Read prof section list
    void readAssociatedProfessorList(ifstream &input);//Read associated professor list
    void readCoursePref(ifstream &input); //Read the course time preferences
    void readProfPref(ifstream &input); //Read professor time preferences
    void readTimeCreditLegend(ifstream &input);// Read time credit legend
    void readCreditTimeslot(ifstream &input); //Read the creditTimeslot array
    void readTimeSlotList(ifstream &input); //Read the timeslot list
    void readInitialSchedule(ifstream &input); //Read the initial schedule
    void readKey(ifstream &input); //Read the key
    void initPopulationFromFirst( ); //Create the initial population from the initial schedule
    
    
    void prepareDataStatistics( ); //Prepare data statistics about input
    int getWeightedRandomIndividual( ); //Get weighted random individual
    string GetFitnessData( ); //Get a string showing the fitness data of the population
    bool allValid( ); //Check if all individuals are valid
};

#endif
