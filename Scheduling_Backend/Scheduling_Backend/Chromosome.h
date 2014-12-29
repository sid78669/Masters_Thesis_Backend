/*
Author: Siddharth Dahiya
Package: Course and Professor Scheduling (Backend)
File: Chromosome.h
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

    void mutate(int ** sectionProf, int ** creditTimeSlot, TimeSlot ** timeSlots, double * timeCredLegend, int timeCredLegendSize, Helper * h, double mutation_probability, double * sectionCredit);
    void repair(int ** sectionProf, int ** creditTimeSlot, TimeSlot ** timeSlots, double * timeCredLegend, int timeCredLegendSize, Helper * h, int ** incompatibleSections, const int REPAIR_TRIES, double * sectionCredit, int ** profSection);
    void updateFitness(int ** incompatibleSections, int ** sectionPref, int ** profPref, TimeSlot ** timeSlots, int timeslot_count, int ** profSectionsTaught);
    void optimize(int ** sectionProf, int ** creditTimeSlot, TimeSlot ** timeSlots, double * timeCredLegend, int timeCredLegendSize, Helper * h, int ** incompatibleSections, double * sectionCredit, int ** profSection);
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
