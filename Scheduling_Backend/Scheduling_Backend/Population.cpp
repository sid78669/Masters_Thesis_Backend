/*
Author: Siddharth Dahiya
Package: Course and Professor Scheduling (Backend)
File: Population.cpp
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


#include "Population.h"

Population::Population(string dataFilePath):
REPAIR_TRIES(100) {
    data_file_path = dataFilePath;
    population_size = 0;
    generation_count = 0;
    replacement_wait = 0;
    mutation_probability = 0;
    section_count = 0;
    professor_count = 0;
    weakestIndividualID = 0;
    course_count = 0;
    professor_count = 0;
    credit_count = 0;
    timeslot_count = 0;
    suffix_cntr = 0;

    while(std::ifstream("stat" + to_string(suffix_cntr) + ".txt")) {
        suffix_cntr++;
    }

    string statFilePath = "stat" + to_string(suffix_cntr) + ".txt";
    string debugFilePath = "debug" + to_string(suffix_cntr) + ".txt";
    string outputFilePath = "output" + to_string(suffix_cntr) + ".txt";
    statFile.open(statFilePath.c_str( ), ofstream::out);
    cout << "Stat file: " << statFilePath << endl;
    debug.open(debugFilePath.c_str( ), ofstream::out);
    cout << "Debug file: " << debugFilePath << endl;
    outputFile.open(outputFilePath.c_str( ), ofstream::out);
    cout << "Output file: " << outputFilePath << endl;
    //Setup the Individuals array
    readDatFiles( );


} //end Constructor

Population::~Population( ) {
    if(statFile.is_open( ))
        statFile.close( );

    if(debug.is_open( ))
        debug.close( );

    if(outputFile.is_open( ))
        outputFile.close( );

    if(individuals) {
        for(int i = 0; i < population_size; ++i) {
            delete individuals[ i ];
        }
        delete[ ] individuals;
    }
    if(timeSlots) {
        for(int i = 0; i < timeslot_count; ++i) {
            delete timeSlots[ i ];
        }
        delete[ ] timeSlots;
    }
    for(int i = 0; i < section_count; ++i) {
        delete[ ] incompatibleSections[ i ];
    } //end for
    delete[ ] incompatibleSections;

    for(int i = 0; i < credit_count; ++i) {
        delete[ ] creditTimeSlot[ i ];
    }

    delete[ ] creditTimeSlot;

    for(int i = 0; i < section_count; ++i) {
        delete[ ] sectionProf[ i ];
    }

    delete[ ] sectionProf;

    for(int i = 0; i < course_count; ++i) {
        delete[ ] courseSection[ i ];
    }

    delete[ ] courseSection;

    delete[ ] sectionCredit;

    for(int i = 0; i < section_count; ++i) {
        delete[ ] sectionPref[ i ];
    }

    delete[ ] sectionPref;

    for(int i = 0; i < professor_count; ++i) {
        delete[ ] profPref[ i ];
    }

    delete[ ] profPref;

    //delete[ ] individualValidity;

    for(int i = 0; i < professor_count; ++i) {
        delete[ ] profSection[ i ];
    }
    delete[ ] profSection;

    delete[ ] profCreditMax;

    delete[ ] timeCredLegend;
} //end destructor

void Population::readDatFiles( ) {
    vector<string> courses;

    ifstream inFile(data_file_path);

    cout << "Beginning Initial Setup...." << endl << endl;

    //Read the parameters
    readParameters(inFile);
    individuals = new Chromosome*[ population_size ];
    //individualValidity = new bool[ population_size ];

    //Read the section List
    readSectionList(inFile, courses);
    cout << endl;
    //Since we have the section count now, initialize all arrays dependent on the number of sections.
    incompatibleSections = new int*[ section_count ];
    sectionProf = new int*[ section_count ];
    sectionCredit = new double[ section_count ];
    sectionPref = new int*[ section_count ];

    //Read the course list
    readCourseList(inFile, courses);
    cout << endl;

    //Now that the courses are setup, time to setup the professor. This is done AFTER courses because each professor is associated with a course.
    readProfessorList(inFile, courses);
    cout << endl;
    profPref = new int*[ professor_count ];

    double courseSum = 0;
    //Sum total credits for courses
    for(int sec = 0; sec < section_count; ++sec) {
        courseSum += sectionCredit[ sec ];
    }

    //Sum total credits for prof
    double profSum = 0;
    for(int prof = 0; prof < professor_count; ++prof) {
        profSum += profCreditMax[ prof ];
    }

    statFile << "Course Credit Sum: " << courseSum << endl;
    statFile << "Prof Credit Sum: " << profSum << endl;

    //The only thing left for initial data, other than an initial chromosome, is timeslots.
    readTimeSlotList(inFile);
    cout << endl;

    /**** Setup soft constraints ****/
    //Read the course time preferences
    readCoursePref(inFile, courses);

    //Read the prof time preferences
    readProfPref(inFile);

    //FINALLY! Use the given chromosome blueprint to create the first chromosome
    readInitialSchedule(inFile);
    cout << endl;
    weakestIndividualID = 0;

    prepareDataStatistics( );

    //individualValidity[ 0 ] = individuals[ 0 ]->isValid();
    statFile << "First Valid: " << individuals[ 0 ]->isValid( ) << endl << endl;

    //Now we have enough basic information that we can populate our individuals based on the first one.
    initPopulationFromFirst( );

    //Output all individuals
    if(DEBUG_INIT_POPULATION)
        for(int i = 0; i < population_size; ++i) {
            debug << "Individual " << i << endl;
            debug << individuals[ i ]->print( );
        } //end for

    inFile.close( );

    cout << endl << "Initial Setup Complete" << endl << endl;
    statFile << "Generation, Replaced, ";
    for(int x = 0; x < population_size; ++x) {
        statFile << to_string(x) << ",";
    }

    statFile << "Mean, SD;" << endl;

    //statFile << "0," << GetFitnessData( );
    statFile << "0," << GetPenaltyData( );
} //end readDatFiles()

void Population::prepareDataStatistics( ) {
    int min = section_count, max = 0, total = 0;
    double mean = 0.0;
    //Get min, max, mean incompatible sections per section
    for(int index = 0; index < section_count; ++index) {
        total += incompatibleSections[ index ][ 0 ];
        if(min > incompatibleSections[ index ][ 0 ]) {
            min = incompatibleSections[ index ][ 0 ];
        }
        if(max < incompatibleSections[ index ][ 0 ]) {
            max = incompatibleSections[ index ][ 0 ];
        }
    }
    mean = ( double ) total / ( double ) section_count;
    statFile << "Incompatible Sections: " << endl;
    statFile << "Range: [" << min << "," << max << "]" << endl;
    statFile << "Mean: " << fixed << setprecision(2) << mean << endl << endl;

    //Get min, max, mean professor per section
    min = professor_count;
    max = 0;
    total = 0;
    mean = 0.0;
    for(int index = 0; index < section_count; ++index) {
        total += sectionProf[ index ][ 0 ];
        if(min > sectionProf[ index ][ 0 ]) {
            min = sectionProf[ index ][ 0 ];
        }
        if(max < sectionProf[ index ][ 0 ]) {
            max = sectionProf[ index ][ 0 ];
        }
    }
    mean = ( double ) total / ( double ) section_count;
    statFile << "Professors Per Section: " << endl;
    statFile << "Range: [" << min << "," << max << "]" << endl;
    statFile << "Mean: " << fixed << setprecision(2) << mean << endl << endl;

    //Get min, max, mean sections per professor
    min = section_count;
    max = 0;
    total = 0;
    mean = 0.0;
    for(int index = 0; index < professor_count; ++index) {
        total += profSection[ index ][ 0 ];
        if(min > profSection[ index ][ 0 ]) {
            min = profSection[ index ][ 0 ];
        }
        if(max < profSection[ index ][ 0 ]) {
            max = profSection[ index ][ 0 ];
        }
    }
    mean = ( double ) total / ( double ) professor_count;
    statFile << "Sections Per Professor: " << endl;
    statFile << "Range: [" << min << "," << max << "]" << endl;
    statFile << "Mean: " << fixed << setprecision(2) << mean << endl << endl;

    //Get min, max, mean timeslots per section
    min = timeslot_count;
    max = 0;
    total = 0;
    mean = 0.0;
    for(int index = 0; index < section_count; ++index) {
        int timeIndex = 0;

        while(timeCredLegend[ timeIndex ] != sectionCredit[ index ])
            timeIndex++;

        total += creditTimeSlot[ timeIndex ][ 0 ];
        if(min > creditTimeSlot[ timeIndex ][ 0 ]) {
            min = creditTimeSlot[ timeIndex ][ 0 ];
        }
        if(max < creditTimeSlot[ timeIndex ][ 0 ]) {
            max = creditTimeSlot[ timeIndex ][ 0 ];
        }
    }
    mean = ( double ) total / ( double ) professor_count;
    statFile << "Timeslots per Section: " << endl;
    statFile << "Range: [" << min << "," << max << "]" << endl;
    statFile << "Mean: " << fixed << setprecision(2) << mean << endl << endl;

    statFile << "Initial Stat Complete..." << endl << endl << endl;

}//end prepareDataStatistics()

void Population::readParameters(ifstream &inFile) {
    string currLine;
    if(inFile.is_open( )) {
        getline(inFile, currLine);
        getline(inFile, currLine);
        generation_count = stoi(currLine);

        getline(inFile, currLine);
        population_size = stoi(currLine);

        getline(inFile, currLine);
        replacement_wait = stoi(currLine);

        getline(inFile, currLine);
        mutation_probability = stod(currLine) / 100.0;
        getline(inFile, currLine);
    }
}

/*
This method will read the section data file. It will then add the list of courses it finds to the vector.
*/
void Population::readSectionList(ifstream &inFile, vector<string> &courseList) {
    cout << "Starting reading sections..." << endl;

    int * tempCourseSections = new int[ ARRAY_MAX ];

    string currLine;
    vector<string> tokenizedVersion;
    if(inFile.is_open( )) {
        while(getline(inFile, currLine)) {
            if(currLine[ 0 ] == '/')
                continue;
            if(currLine.compare("*END*SECTION*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            courseList.push_back(tokenizedVersion[ 0 ]);
            int sections = stoi(tokenizedVersion[ 1 ]);
            tempCourseSections[ courseList.size( ) - 1 ] = sections;
            if(DEBUG_SECTION)
                debug << currLine << "> " << tokenizedVersion[ 0 ] << "#" << tokenizedVersion[ 1 ] << "** " << endl;
        } //end while
    } //end if

    //Initialize the courseSection array and populate it.
    courseSection = new int*[ courseList.size( ) ];
    for(int i = 0; i < ( signed ) courseList.size( ); ++i) {
        courseSection[ i ] = new int[ tempCourseSections[ i ] + 1 ];
        courseSection[ i ][ 0 ] = tempCourseSections[ i ];
        if(DEBUG_SECTION)
            debug << courseList.at(i) << ">> (" << courseSection[ i ][ 0 ] << ") ";

        for(int j = 1; j <= courseSection[ i ][ 0 ]; ++j) {
            courseSection[ i ][ j ] = section_count++;
            if(DEBUG_SECTION)
                debug << courseSection[ i ][ j ] << "_";
        } //end for

        if(DEBUG_SECTION)
            debug << endl;
    } //end for

    //delete the temp helper array
    delete[ ] tempCourseSections;
    course_count = courseList.size( );
    statFile << "New Courses Added: " << course_count << endl
        << "New Sections Added: " << section_count << endl;
} //end readSectionList

/*
This method will readh the course data file. It will then populate the incompatible section array and the sectionCredit arrays.
*/
void Population::readCourseList(ifstream &inFile, vector<string> courseList) {
    cout << "Starting reading courses..." << endl;

    string currLine;
    vector<string> tokenizedVersion;
    if(inFile.is_open( )) {
        while(getline(inFile, currLine)) {
            if(currLine[ 0 ] == '/')
                continue;
            if(currLine.compare("*END*COURSE*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');

            //Find the courseID
            int courseLoc = find(courseList.begin( ), courseList.end( ),
                                 tokenizedVersion.at(0)) - courseList.begin( );

            //Setup data that will be same between related sections, i.e. credits and incompatible sections.
            double cred = stod(tokenizedVersion.at(1));
            vector<int> incompatibles;
            //Check if there are any incompatible sections.
            if(tokenizedVersion.size( ) > 2) {
                for(int inc = 2; inc < ( signed ) tokenizedVersion.size( );
                ++inc) {
               //Find the courseID
                    int currIncomp = find(courseList.begin( ), courseList.end( ),
                                          tokenizedVersion.at(0)) - courseList.begin( );

                    //Add the half sections for this course to the vector
                    for(int secIdx = 1;
                    secIdx <= courseSection[ currIncomp ][ 0 ] / 2;
                        ++secIdx) {
                        incompatibles.push_back(
                            courseSection[ currIncomp ][ secIdx ]);
                    } //end for
                } //end for
            } //end if

            if(DEBUG_COURSES) {
                for(auto inco : incompatibles) {
                    debug << inco << ", ";
                }
                debug << endl;
            }

            //Loop through the sections and set the values for credits and incompatible sections
            for(int i = 1; i <= courseSection[ courseLoc ][ 0 ]; ++i) {
                if(DEBUG_COURSES)
                    debug << courseSection[ courseLoc ][ i ] << ": ";
                int sectionID = courseSection[ courseLoc ][ i ];
                sectionCredit[ sectionID ] = cred;

                //Copy all the incompatible sections over.
                incompatibleSections[ sectionID ] = new int[ incompatibles.size( ) + 1 ];
                incompatibleSections[ sectionID ][ 0 ] = incompatibles.size( );
                for(int j = 1; j <= ( signed ) incompatibles.size( ); ++j) {
                    if(DEBUG_COURSES)
                        debug << ", " << incompatibles.at(j - 1);
                    incompatibleSections[ sectionID ][ j ] = incompatibles.at(j - 1);
                } //end for
                if(DEBUG_COURSES)
                    debug << endl;
            } //end for
        } //end while
    } //end if

    statFile << "Finished reading courses. Updated " << course_count << " new courses." << endl;
} //end readCourseList

void Population::readProfessorList(ifstream &inFile, vector<string> courseList) {
    cout << "Starting reading professors...." << endl;

    string currLine;
    vector<int> * secProfs = new vector<int>[ section_count ];

    vector<double> maxCredits;
    vector<string> tokenizedVersion;
    if(inFile.is_open( )) {
        while(getline(inFile, currLine)) {
            if(currLine[ 0 ] == '/')
                continue;
            if(currLine.compare("*END*PROFESSOR*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);

            tokenizedVersion = Utility::Tokenize(currLine, ',');
            tokenizedVersion.erase(tokenizedVersion.begin( ));
            maxCredits.push_back(stod(tokenizedVersion[ 0 ]));
            tokenizedVersion.erase(tokenizedVersion.begin( ));
            for(int courseLoc = 0; courseLoc < ( signed ) tokenizedVersion.size( ); ++courseLoc) {
                int courseID = find(courseList.begin( ), courseList.end( ),
                                    tokenizedVersion.at(courseLoc)) - courseList.begin( );
                if(DEBUG_PROF)
                    debug << "Course: " << tokenizedVersion.at(courseLoc) << " ID: " << courseID << endl;
                for(int sects = 1; sects <= courseSection[ courseID ][ 0 ];
                ++sects) {
                    secProfs[ courseSection[ courseID ][ sects ] ].push_back(professor_count);
                    if(DEBUG_PROF)
                        debug << "Section: " << courseSection[ courseID ][ sects ] << " Prof: " << professor_count << endl;
                }
            }
            professor_count++;
        }

        profSection = new int *[ section_count ];
        profCreditMax = new double[ professor_count ]( );
        vector<int> * profSectionVector = new vector<int>[ professor_count ];
        for(int z = 0; z < professor_count; z++) {
            profCreditMax[ z ] = maxCredits[ z ];
        }
        for(int i = 0; i < section_count; ++i) {
            if(DEBUG_PROF)
                debug << i << "---" << endl;
            sectionProf[ i ] = new int[ secProfs[ i ].size( ) + 1 ];
            sectionProf[ i ][ 0 ] = secProfs[ i ].size( );
            int cntr = 1;
            for(auto sp : secProfs[ i ]) {
                sectionProf[ i ][ cntr++ ] = sp;
                profSectionVector[ sp ].push_back(i);
                if(DEBUG_PROF)
                    debug << sp << ", ";
            }
            if(DEBUG_PROF)
                debug << endl;
        }
        for(int prof = 0; prof < professor_count; ++prof) {
            profSection[ prof ] = new int[ profSectionVector[ prof ].size( ) + 1 ];
            profSection[ prof ][ 0 ] = profSectionVector[ prof ].size( );
            int l = 1;
            for(auto sect : profSectionVector[ prof ]) {
                profSection[ prof ][ l++ ] = sect;
            }
        }
        if(DEBUG_PROF) {
            debug << "Prof Sections taught: " << endl;
            for(int i = 0; i < professor_count; i++) {
                debug << "(" << i << "," << profSection[ i ][ 0 ] << ")" << endl;
            }
        }
    }

    delete[ ] secProfs;

    statFile << "Number of Professors added: " << professor_count << endl;
}

void Population::readTimeSlotList(ifstream &inFile) {
    cout << "Starting reading TimeSlots" << endl;
    vector<vector<int>> cTimeSlot;

    vector<string> tokenizedVersion;
    string currLine;
    int timeslotID = 0;
    TimeSlot ** tempArray = new TimeSlot*[ ARRAY_MAX ];
    if(inFile.is_open( )) {
        while(getline(inFile, currLine)) {
            if(currLine[ 0 ] == '/')
                continue;
            if(currLine.compare("*END*TIMESLOT*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            timeslotID = stoi(tokenizedVersion.at(0));
            while(timeslot_count < timeslotID) {
                tempArray[ timeslot_count ] = new TimeSlot(-1);
                int creditLoc = 0;
                while(!cTimeSlot.empty( )
                      && creditLoc < ( signed ) cTimeSlot.size( )
                      && tempArray[ cTimeSlot.at(creditLoc).at(0) ]->getCredits( )
                      != -1)
                      creditLoc++;
                if(creditLoc >= ( signed ) cTimeSlot.size( )) {
                    vector<int> newVec;
                    cTimeSlot.push_back(newVec);
                }
                cTimeSlot.at(creditLoc).push_back(timeslot_count);
                timeslot_count++;
            }
            double creds = stod(tokenizedVersion.at(1));
            tempArray[ timeslot_count ] = new TimeSlot(creds);
            for(int i = 0; i < 6; ++i)
                tempArray[ timeslot_count ]->setPeriod(i,
                tokenizedVersion.at(2 + i));
            int creditLoc = 0;
            while(!cTimeSlot.empty( ) && creditLoc < ( signed ) cTimeSlot.size( )
                  && tempArray[ cTimeSlot.at(creditLoc).at(0) ]->getCredits( )
                  != creds)
                  creditLoc++;
            if(creditLoc >= ( signed ) cTimeSlot.size( )) {
                vector<int> newVec;
                cTimeSlot.push_back(newVec);
            }
            cTimeSlot.at(creditLoc).push_back(timeslot_count);
            timeslot_count++;
        }

        timeSlots = new TimeSlot*[ timeslot_count ];
        for(int i = 0; i < timeslot_count; ++i) {
            timeSlots[ i ] = new TimeSlot(( *tempArray[ i ] ));
            delete tempArray[ i ];
            if(DEBUG_TIMESLOT)
                debug << i << " - " << timeSlots[ i ]->print( ) << endl;
        }

        credit_count = cTimeSlot.size( );
        creditTimeSlot = new int*[ credit_count ];
        timeCredLegend = new double[ credit_count ];
        for(int i = 0; i < credit_count; ++i) {
            timeCredLegend[ i ] = timeSlots[ cTimeSlot.at(i).at(0) ]->getCredits( );
            creditTimeSlot[ i ] = new int[ cTimeSlot.at(i).size( ) + 1 ];
            creditTimeSlot[ i ][ 0 ] = cTimeSlot.at(i).size( );
            for(int j = 1; j <= creditTimeSlot[ i ][ 0 ]; ++j) {
                creditTimeSlot[ i ][ j ] = cTimeSlot.at(i).at(j - 1);
            }
        }
    }

    delete[ ] tempArray;

    statFile << "New TimeSlots Added: " << timeslot_count << endl;
}

void Population::readInitialSchedule(ifstream &inFile) {
    cout << "Initiating First Chromosome..." << endl;

    vector<string> tokenizedVersion;
    string currLine;
    individuals[ 0 ] = new Chromosome(section_count, professor_count,
                                      profCreditMax);

    int geneLocation = 0;
    if(inFile.is_open( )) {
        while(getline(inFile, currLine)) {
            if(currLine[ 0 ] == '/')
                continue;
            if(currLine.compare("*END*INITIAL*") == 0)
                break;

            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');

            Gene current(stoi(tokenizedVersion.at(1)),
                         stoi(tokenizedVersion.at(2)));
            individuals[ 0 ]->setGene(geneLocation, current);
            individuals[ 0 ]->setProf(geneLocation, stoi(tokenizedVersion.at(1)),
                                      sectionCredit[ geneLocation ]);
            geneLocation++;
        }
        if(DEBUG_INIT)
            debug << individuals[ 0 ]->print( ) << endl;
    }
    //individuals[ 0 ]->updateFitness(incompatibleSections, sectionPref, profPref, timeSlots, timeslot_count, profSection);
    individuals[ 0 ]->updatePenalty(incompatibleSections, sectionPref, profPref, timeSlots, timeslot_count, profSection);

    weakestIndividualID = 0;
    //lowestFitnessSeen = individuals[ 0 ]->getFitness( );
    highestPenaltySeen = individuals[ 0 ]->getPenalty( );
    strongestIndividualID = 0;
    //highestFitnessSeen = individuals[ 0 ]->getFitness( );
    lowestPenaltySeen = individuals[ 0 ]->getPenalty( );

    statFile << "Prof Rating: " << endl << individuals[ 0 ]->getProfessorLoads( ) << endl;
    //initFitness = ( individuals[ 0 ]->getFitness( ) *100.0 / MAX_FITNESS*1.0 );
    initPenalty = ( individuals[ 0 ]->getPenalty( ) );
    //statFile << "Initial Schedule Fitness: " << initFitness << "%" << endl;
    statFile << "Initial Schedule Penalty: " << initPenalty << endl;
    cout << "First Chromosome Initialized." << endl;
}

void Population::initPopulationFromFirst( ) {
    cout << "Initiating generation of population...." << endl;
    double original_mutation_probability = mutation_probability;
    mutation_probability = 0.1;

    for(int i = 1; i < population_size; ++i) {
        if(DEBUG_INIT_POPULATION)
            debug << "Generating individual (" << to_string(i) << ")..." << endl;

        individuals[ i ] = new Chromosome(individuals[ 0 ]);
        if(DEBUG_INIT_POPULATION_COMPARED)
            debug << "Pre-Mutation: " << endl << individuals[ i ]->print( );

        individuals[ i ]->mutate(sectionProf, creditTimeSlot, timeSlots, timeCredLegend, credit_count, &h,
                                 mutation_probability, sectionCredit);

        if(DEBUG_INIT_POPULATION)
            debug << "Post-Mutation: " << endl << individuals[ i ]->print( ) << endl;

            //After mutation, repair.
        individuals[ i ]->repair(sectionProf, creditTimeSlot, timeSlots, timeCredLegend, credit_count, &h,
                                 incompatibleSections, REPAIR_TRIES, sectionCredit, profSection);

        if(DEBUG_INIT_POPULATION)
            debug << "Post-Repair: " << endl << individuals[ i ]->print( ) << endl;

            //individuals[ i ]->updateFitness(incompatibleSections, sectionPref,profPref, timeSlots, timeslot_count,profSection);
        individuals[ i ]->updatePenalty(incompatibleSections, sectionPref, profPref, timeSlots, timeslot_count, profSection);

        if(DEBUG_INIT_POPULATION)
            debug << i << " valid: " << individuals[ i ]->isValid( ) << endl;
            //debug << "Fitness for " << i << " is " << individuals[ i ]->getFitness( ) << endl << endl;
        if(DEBUG_INIT_POPULATION)
            debug << "Penalty for " << i << " is " << individuals[ i ]->getPenalty( ) << endl << endl;

            //if(individuals[ i ]->getFitness( ) < individuals[ weakestIndividualID ]->getFitness( )) {
            //    weakestIndividualID = i;
            //    lowestFitnessSeen = individuals[ i ]->getFitness( );
            //}
        if(individuals[ i ]->getPenalty( ) > individuals[ weakestIndividualID ]->getPenalty( )) {
            weakestIndividualID = i;
            highestPenaltySeen = individuals[ i ]->getPenalty( );
        }

        //if(individuals[ i ]->getFitness( ) > individuals[ strongestIndividualID ]->getFitness( )) {
        //    strongestIndividualID = i;
        //    highestFitnessSeen = individuals[ i ]->getFitness( );
        //}
        if(individuals[ i ]->getPenalty( ) < individuals[ strongestIndividualID ]->getPenalty( )) {
            strongestIndividualID = i;
            lowestPenaltySeen = individuals[ i ]->getPenalty( );
        }
    }
    mutation_probability = original_mutation_probability;

    cout << "Population generated." << endl;
}

void Population::readCoursePref(ifstream &inFile, vector<string> courseList) {
    string currLine;
    //1. Create a temp coursePref list that will be used to populate section pref
    //2. populate the coursePref list using a similar approach as to populating course data
    //3. populate sectionPref from the coursePref

    vector<string> tokenizedVersion;
    if(inFile.is_open( )) {
        while(getline(inFile, currLine)) {
            if(currLine[ 0 ] == '/')
                continue;
            if(currLine.compare("*END*COURSEPREF*") == 0)
                break;

            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int courseIndex = find(courseList.begin( ), courseList.end( ),
                                   tokenizedVersion.at(0)) - courseList.begin( );
            if(courseIndex < ( signed ) courseList.size( )) {
                string prefs = tokenizedVersion.at(1);
                Utility::RemoveChar(prefs, '(');
                Utility::RemoveChar(prefs, ')');
                tokenizedVersion = Utility::Tokenize(prefs, '-');
                for(int secIdx = 1; secIdx <= courseSection[ courseIndex ][ 0 ];
                ++secIdx) {
                    int sectionID = courseSection[ courseIndex ][ secIdx ];
                    sectionPref[ sectionID ] = new int[ 3 ];
                    for(int idx = 0; idx < 3; ++idx) {
                        sectionPref[ courseSection[ courseIndex ][ secIdx ] ][ idx ] =
                            stoi(tokenizedVersion.at(idx));
                    } //end for idx
                } // end for section
            }
            else {
                continue;
            } //(courseIndex < courseList.size( ))
        } //while(getline(inFile, currLine))
    } //if (inFile.is_open( ))

}

void Population::readProfPref(ifstream &inFile) {
    string currLine;
    vector<string> tokenizedVersion;

    if(inFile.is_open( )) {
        while(getline(inFile, currLine)) {
            if(currLine[ 0 ] == '/')
                continue;
            if(currLine.compare("*END*PROFPREF*") == 0)
                break;

            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);

            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int profIndex = stoi(tokenizedVersion.at(0));
            profPref[ profIndex ] = new int[ 3 ];
            string prefs = tokenizedVersion.at(1);
            Utility::RemoveChar(prefs, '(');
            Utility::RemoveChar(prefs, ')');
            tokenizedVersion = Utility::Tokenize(prefs, '-');
            for(int idx = 0; idx < 3; ++idx) {
                profPref[ profIndex ][ idx ] = stoi(tokenizedVersion.at(idx));
            } //end for

        } //end while (getline(inFile, currLine))

    } //end if(inFile.is_open( ))
}

int Population::getWeightedRandomIndividual( ) {
    int minWeight = 0;
    int maxWeight = 0;
    for(int i = 0; i < population_size; i++) {
        //maxWeight += abs(individuals[ i ]->getFitness( ));
        maxWeight += individuals[ i ]->getPenalty( );
        if(maxWeight > (int)pow(2, 30))
        {
            maxWeight = -1;
            break;
        }
    }

    if(maxWeight > 0) {
        int randomWeight = h.randNum(minWeight, maxWeight);
        int sacrifice = 0;
        for(sacrifice = 0; sacrifice < population_size; ++sacrifice) {
            //maxWeight -= abs(individuals[ sacrifice ]->getFitness( ));
            randomWeight -= individuals[ sacrifice ]->getPenalty( );
            if(randomWeight <= 0)
                break;
        }
        return sacrifice;
    }
    else {
        return h.randNum(0, population_size - 1);
    }
}

bool Population::allValid( ) {
    for(int i = 0; i < population_size; ++i) {
        if(!individuals[ i ]->isValid( )) {
            return false;
        }
    }
    return true;
}

bool Population::allValid(int currentGeneration) {
    bool val = allValid( );
    if(val) {
        cout << "Valid population at " << currentGeneration << endl;
    }

    return val;
}


void Population::Evolve( ) {
    int generationSinceLastReplacement = 0;

    int currentGeneration = 1;
    int threshold_generation = ( int ) ( ( double ) generation_count * 1 );
    cout << "Total generations to evolve over: " << generation_count << endl;
    cout << "Threshold for invalid individuals " << threshold_generation << endl;
    cout << "Current Generation: ";
    int len = to_string(generation_count).length( );

    int generationOfFullValidity = generation_count + 1;

    for(currentGeneration = 0; currentGeneration <= generation_count; ++currentGeneration) {
        //Output only every 50 generations
        if(currentGeneration == 0) {
            cout << setw(len) << "0";
        }
        else {
            for(int b = 0; b < len; ++b) {
                cout << '\b';
            }
            cout << setw(len) << to_string(currentGeneration);
        }

        if(generationSinceLastReplacement <= 1 && DEBUG_EVOLVE) {
            debug << "Current Generation: " << currentGeneration << " Generations since last replacement: " << generationSinceLastReplacement;
            if(generationSinceLastReplacement == 1) {
                debug << "...";
            }
            else
                debug << endl;
        }

        int sacrificeID = getWeightedRandomIndividual( );

        if(individuals[ sacrificeID ]->getPenalty( ) < lowestPenaltySeen)
            lowestPenaltySeen = individuals[ sacrificeID ]->getPenalty( );
        if(individuals[ sacrificeID ]->getPenalty( ) > highestPenaltySeen)
            highestPenaltySeen = individuals[ sacrificeID ]->getPenalty( );

        Chromosome * sacrifice = new Chromosome(individuals[ sacrificeID ]);

        if(DEBUG_EVOLVE) {
            debug << "Sacrifice source: " << sacrificeID << endl;
            debug << "Sacrifice: " << endl << sacrifice->print( ) << "Source: " << endl << individuals[ sacrificeID ]->print( ) << endl;
        }

        sacrifice->mutate(sectionProf, creditTimeSlot, timeSlots, timeCredLegend, credit_count, &h, mutation_probability,
                          sectionCredit);
        sacrifice->repair(sectionProf, creditTimeSlot, timeSlots, timeCredLegend, credit_count, &h, incompatibleSections,
                          REPAIR_TRIES, sectionCredit, profSection);
        //sacrifice->updateFitness(incompatibleSections, sectionPref, profPref, timeSlots, timeslot_count, profSection);
        sacrifice->updatePenalty(incompatibleSections, sectionPref, profPref, timeSlots, timeslot_count, profSection);

        if(currentGeneration > threshold_generation && !sacrifice->isValid( )) {
            currentGeneration--;
            delete sacrifice;
            continue;
        }
        int replaced = weakestIndividualID;
        //if(sacrifice->getFitness( ) > individuals[ weakestIndividualID ]->getFitness( )) {
        if(sacrifice->getPenalty( ) < individuals[ weakestIndividualID ]->getPenalty( )) {

            //Only optimize if the sacrifice is valid.
            if(sacrifice->isValid( )) {
                sacrifice->optimize(sectionProf, creditTimeSlot, timeSlots, timeCredLegend, credit_count, &h, incompatibleSections,
                                    sectionCredit, profSection, sectionPref, profPref, timeslot_count);
                if(allValid(currentGeneration) && generationOfFullValidity > currentGeneration && currentGeneration > threshold_generation) {
                    statFile << endl << endl << "All Valid: " << currentGeneration << endl << endl;
                    generationOfFullValidity = currentGeneration;
                }
            }

            if(DEBUG_EVOLVE) {
                if(generationSinceLastReplacement > 1) {
                    debug << endl << "Current Generation: " << currentGeneration << " Generations since last replacement: "
                        << generationSinceLastReplacement << endl;
                }
                else if(generationSinceLastReplacement == 1) {
                    debug << " Replaced" << endl;
                }
                else if(generationSinceLastReplacement == 0) {
                    debug << endl;
                }
            }
            generationSinceLastReplacement = 0;

            if(DEBUG_EVOLVE) {
                debug << "Replacing " << weakestIndividualID << " with the sacrifice." << endl;
                debug << "Original\t\tNew" << endl;
                for(int indexB = 0; indexB < section_count; ++indexB)
                    debug << individuals[ weakestIndividualID ]->print(indexB)
                    << "  ->  " << sacrifice->print(indexB) << endl;
                debug << "New weakest: " << endl;
            }

            delete individuals[ weakestIndividualID ];
            individuals[ weakestIndividualID ] = new Chromosome(sacrifice);

            if(DEBUG_EVOLVE)
                debug << individuals[ weakestIndividualID ]->print( );
            bool valid_population = allValid( );
            //If we are past the threshold generation, we want to start creating a valid population.
            if(currentGeneration > threshold_generation && !valid_population) {
                int weakestInvalid = -1;
                //int lowestFitness = lowestFitnessSeen;
                //int highestPenalty = highestPenaltySeen;
                highestPenaltySeen = 0;
                for(int j = 0; j < population_size; ++j) {
                    if(!individuals[ j ]->isValid( ) && individuals[ j ]->getPenalty( ) > highestPenaltySeen) {
                        weakestInvalid = j;
                        highestPenaltySeen = individuals[ j ]->getPenalty( );
                    }
                    if(individuals[ j ]->getPenalty( ) < individuals[ strongestIndividualID ]->getPenalty( )) {
                        strongestIndividualID = j;
                    }
                }
                if(weakestInvalid != -1)
                    weakestIndividualID = weakestInvalid;
                else
                    goto alternateWeakestSelection;
            }
            else {
            alternateWeakestSelection:
                for(int j = 0; j < population_size; ++j) {
                    if(individuals[ j ]->getPenalty( ) > individuals[ weakestIndividualID ]->getPenalty( )) {
                        weakestIndividualID = j;
                    }
                    if(individuals[ j ]->getPenalty( ) < individuals[ strongestIndividualID ]->getPenalty( )) {
                        strongestIndividualID = j;
                    }
                }
            }
        }
        else {
            //generationSinceLastReplacement++;
            //if(generationSinceLastReplacement < replacement_wait) {
            currentGeneration--;
            delete sacrifice;
            continue;
        //}
        }

        if(DEBUG_EVOLVE) {
            debug << "Current State" << endl;
            for(int indexC = 0; indexC < population_size; ++indexC) {
                debug << "Individual " << indexC << endl;
                debug << individuals[ indexC ]->print( );
            }
            debug << "-----------------------------------------------------------" << endl;
        }
        delete sacrifice;

        if(currentGeneration > 0) {// && ( ( i % 100 == 0 ) || i == 1 )) {
            statFile << currentGeneration << "," << replaced << ",";
            //statFile << GetFitnessData( );
            statFile << GetPenaltyData( );
        }
    }
    cout << endl << "Evolution Complete." << endl;
    statFile << currentGeneration << ",";
    //statFile << GetFitnessData( ) << endl;
    statFile << GetPenaltyData( ) << endl;
    statFile << "Population valid at: " << generationOfFullValidity << endl;
}

string Population::PrintTableFormat( ) {
    string rtnVal = "";

    if(DEBUG_PRINTTABLE) {
        for(int i = 0; i < 1; ++i) {
            rtnVal += individuals[ i ]->printTable(timeSlots, timeslot_count, sectionCredit);
            rtnVal += "\n\n";
        }
    }
    else {
        for(int i = 0; i < population_size; ++i) {
            rtnVal += individuals[ i ]->printTable(timeSlots, timeslot_count);
            rtnVal += "\n\n";
        }
    }

    return rtnVal;
}

//string Population::GetFitnessData( ) {
//    int * allFitness = new int[ population_size - 2 ];
//    stringstream s;
//    for(int i = 0; i < population_size; ++i) {
//        allFitness[ i - 1 ] = individuals[ i ]->getFitness( );
//        s << individuals[ i ]->getFitness( ) << ",";
//    }
//    double mean = Utility::CalculateMean(allFitness, population_size - 1);
//    double sd = Utility::StandardDeviation(allFitness, population_size - 1, mean);
//    s << mean << "," << sd;
//    s << ";\n";
//    return s.str( );
//}

string Population::GetPenaltyData( ) {
    int * allPenalty = new int[ population_size - 2 ];
    stringstream s;
    for(int i = 0; i < population_size; ++i) {
        allPenalty[ i - 1 ] = individuals[ i ]->getPenalty( );
        s << individuals[ i ]->getPenalty( ) << ",";
    }
    double mean = Utility::CalculateMean(allPenalty, population_size - 1);
    double sd = Utility::StandardDeviation(allPenalty, population_size - 1, mean);
    s << mean << "," << sd;
    s << ";\n";
    return s.str( );
}

void Population::PrintEnd( ) {
    outputFile << "Final Population:" << endl;
    //double strongestFitness = ( individuals[ strongestIndividualID ]->getFitness( )*100.0 / MAX_FITNESS*1.0 );
    double lowestPenalty = individuals[ strongestIndividualID ]->getPenalty( );
    //strongestFitness = ( strongestFitness < 0 ? strongestFitness * -1.0 : strongestFitness );
    if(PRINT_WHOLE_POPULATION) {
        outputFile << "Printing Current Population..." << endl;
        int* penaltyValues = new int[ population_size ];
        for(int i = 0; i < population_size; ++i) {
            outputFile << "Individual " << i << endl;
            outputFile
                << individuals[ i ]->printTable(timeSlots,
                timeslot_count, sectionCredit);
            penaltyValues[ i ] = individuals[ i ]->getPenalty( );
            outputFile << "Penatly: " << penaltyValues[ i ] << endl;
            outputFile << "Valid: "
                << ( individuals[ i ]->isValid( ) ?
                "Yes" : "No" ) << endl;
            outputFile << "*******************************************************"
                << endl << endl;
        }
        outputFile << "Current penalty values: " << endl;
        for(int i = 0; i < population_size - 1; ++i) {
            outputFile << penaltyValues[ i ] << ",";
        }
        outputFile << penaltyValues[ population_size - 1 ] << endl;
    } //if (PRINT_WHOLE_POPULATION)

    outputFile << "\n\nStrongest Individual: " << strongestIndividualID << endl;
    outputFile << individuals[ strongestIndividualID ]->printTable(timeSlots, timeslot_count);
    //outputFile << "Fitness: " << strongestFitness << "%" << endl;
    outputFile << "Penalty: " << lowestPenalty << endl;
    double improvement = ( lowestPenalty + initPenalty ) *100.0 / initPenalty;

    if(improvement < 0)
        improvement *= -1.0;
    outputFile << "Improvement from Initial Schedule: " << improvement << "%" << endl;
    outputFile << "Valid: " << ( individuals[ strongestIndividualID ]->isValid( ) ? "Yes" : "No" ) << endl;

    outputFile << "Professor Schedule: " << endl << individuals[ strongestIndividualID ]->printProfTable( ) << endl;

    if(!individuals[ strongestIndividualID ]->isValid( )) {
        double strongestValid = 0.0;
        int maxFitness = 0, maxFitID = 0;
        bool validFound = false;
        for(int i = 0; i < population_size; ++i) {
            if(individuals[ i ]->isValid( ) && individuals[ i ]->getPenalty( ) > maxFitness) {
                validFound = true;
                maxFitID = i;
                maxFitness = individuals[ i ]->getPenalty( );
            }
        }
        if(maxFitness == 0) {
            maxFitness = individuals[ maxFitID ]->getPenalty( );
        }
        if(maxFitness < individuals[ strongestIndividualID ]->getPenalty( )) {
            outputFile << "\n\n Strongest individual is the that was found.\n" << endl;
        }
        else {
            strongestValid = ( maxFitness*100.0 / MAX_FITNESS*1.0 );
            strongestValid = ( strongestValid < 0 ? strongestValid * -1.0 : strongestValid );
            outputFile << "\n\nStrongest Valid Individual: " << maxFitID << endl;
            outputFile << individuals[ maxFitID ]->printTable(timeSlots,
                                                              timeslot_count);
            outputFile << "Fitnesss: " << strongestValid << "%" << endl;
            outputFile << "Valid: " << ( validFound ? "Yes" : "No" ) << endl;
            outputFile << "Professor Schedule: " << endl << individuals[ maxFitID ]->printProfTable( ) << endl;
            improvement = ( strongestValid + initPenalty ) *100.0 / initPenalty;

            if(improvement < 0)
                improvement *= -1.0;
            outputFile << "Improvement from Initial Schedule: " << improvement << "%" << endl;
        }
    }
    else {
        outputFile << "Strongest was valid." << endl;
    }

    outputFile << "Done." << endl;
    statFile.close( );
    //outputFile.close( );
}

ostream & operator<<( ostream & os, const Population &source ) {
    //double strongestFitness = ( source.individuals[ source.strongestIndividualID ]->getFitness( )*100.0 / MAX_FITNESS*1.0 );
    double lowestPenalty = source.individuals[ source.strongestIndividualID ]->getPenalty( );
    //strongestFitness = ( strongestFitness < 0 ? strongestFitness * -1.0 : strongestFitness );
    if(PRINT_WHOLE_POPULATION) {
        os << "Printing Current Population..." << endl;
        int* penaltyValues = new int[ source.population_size ];
        for(int i = 0; i < source.population_size; ++i) {
            os << "Individual " << i << endl;
            os << source.individuals[ i ]->printTable(source.timeSlots,
                                                      source.timeslot_count, source.sectionCredit);
            penaltyValues[ i ] = source.individuals[ i ]->getPenalty( );
            os << "Penalty: " << penaltyValues[ i ] << endl;
            os << "Valid: "
                << ( source.individuals[ i ]->isValid( ) ?
                "Yes" : "No" ) << endl;
            os << "*******************************************************"
                << endl << endl;
        }
        os << "Current penalty values: " << endl;
        for(int i = 0; i < source.population_size - 1; ++i) {
            os << penaltyValues[ i ] << ",";
        }
        os << penaltyValues[ source.population_size - 1 ] << endl;
    } //if (PRINT_WHOLE_POPULATION)

    os << "\n\nStrongest Individual: " << source.strongestIndividualID << endl;
    os << source.individuals[ source.strongestIndividualID ]->printTable(source.timeSlots, source.timeslot_count);
    //os << "Fitness: "<< strongestFitness << "%"<< endl;
    os << "Penalty: " << lowestPenalty << endl;
    double improvement = ( lowestPenalty + source.initPenalty ) *100.0 / source.initPenalty;

    if(improvement < 0)
        improvement *= -1.0;
    os << "Improvement from Initial Schedule: " << improvement << "%" << endl;
    os << "Valid: " << ( source.individuals[ source.strongestIndividualID ]->isValid( ) ? "Yes" : "No" ) << endl;

    os << "Professor Schedule: " << endl << source.individuals[ source.strongestIndividualID ]->printProfTable( ) << endl;

    //if(!source.individuals[ source.strongestIndividualID ]->isValid( )) {
    //    double strongestValid = 0.0;
    //    int maxFitness = 0, maxFitID = 0;
    //    bool validFound = false;
    //    for(int i = 0; i < source.population_size; ++i) {
    //        if(source.individuals[ i ]->isValid( ) && source.individuals[ i ]->getFitness( ) > maxFitness) {
    //            validFound = true;
    //            maxFitID = i;
    //            maxFitness = source.individuals[ i ]->getFitness( );
    //        }
    //    }
    //    if(maxFitness == 0) {
    //        maxFitness = source.individuals[ maxFitID ]->getFitness( );
    //    }
    //    if(maxFitness < source.individuals[ source.strongestIndividualID ]->getFitness( )) {
    //        os << "\n\n Strongest individual is the that was found.\n" << endl;
    //    }
    //    else {
    //        strongestValid = ( maxFitness*100.0 / MAX_FITNESS*1.0 );
    //        strongestValid = ( strongestValid < 0 ? strongestValid * -1.0 : strongestValid );
    //        os << "\n\nStrongest Valid Individual: " << maxFitID << endl;
    //        os << source.individuals[ maxFitID ]->printTable(source.timeSlots,
    //                                                         source.timeslot_count);
    //        os << "Fitnesss: " << strongestValid << "%" << endl;
    //        os << "Valid: " << ( validFound ? "Yes" : "No" ) << endl;
    //        os << "Professor Schedule: " << endl << source.individuals[ maxFitID ]->printProfTable( ) << endl;
    //        improvement = ( strongestValid + abs(source.initFitness) ) *100.0 / abs(source.initFitness);

    //        if(improvement < 0)
    //            improvement *= -1.0;
    //        os << "Improvement from Initial Schedule: " << improvement << "%" << endl;
    //    }
    //}
    //else {
    //    os << "Strongest was valid." << endl;
    //}

    os << "Done." << endl;
    return os;
}


//ostream & operator<<( ostream & os, const Population &source ) {
//    double strongestFitness = ( source.individuals[ source.strongestIndividualID ]->getFitness( )*100.0 / MAX_FITNESS*1.0 );
//    strongestFitness = ( strongestFitness < 0 ? strongestFitness * -1.0 : strongestFitness );
//    if(PRINT_WHOLE_POPULATION) {
//        os << "Printing Current Population..." << endl;
//        int* penaltyValues = new int[ source.population_size ];
//        for(int i = 0; i < source.population_size; ++i) {
//            os << "Individual " << i << endl;
//            os
//                << source.individuals[ i ]->printTable(source.timeSlots,
//                source.timeslot_count, source.sectionCredit);
//            penaltyValues[ i ] = source.individuals[ i ]->getFitness( );
//            os << "Fitnesss: " << penaltyValues[ i ] << endl;
//            os << "Valid: "
//                << ( source.individuals[ i ]->isValid( ) ?
//                "Yes" : "No" ) << endl;
//            os << "*******************************************************"
//                << endl << endl;
//        }
//        os << "Current fitness values: " << endl;
//        for(int i = 0; i < source.population_size - 1; ++i) {
//            os << penaltyValues[ i ] << ",";
//        }
//        os << penaltyValues[ source.population_size - 1 ] << endl;
//    } //if (PRINT_WHOLE_POPULATION)
//
//    os << "\n\nStrongest Individual: " << source.strongestIndividualID << endl;
//    os
//        << source.individuals[ source.strongestIndividualID ]->printTable(
//        source.timeSlots, source.timeslot_count);
//    os << "Fitness: "
//        << strongestFitness << "%"
//        << endl;
//    double improvement = ( strongestFitness + abs(source.initFitness) ) *100.0 / abs(source.initFitness);
//
//    if(improvement < 0)
//        improvement *= -1.0;
//    os << "Improvement from Initial Schedule: " << improvement << "%" << endl;
//    os << "Valid: " << ( source.individuals[ source.strongestIndividualID ]->isValid( ) ? "Yes" : "No" ) << endl;
//
//    os << "Professor Schedule: " << endl << source.individuals[ source.strongestIndividualID ]->printProfTable( ) << endl;
//
//    if(!source.individuals[ source.strongestIndividualID ]->isValid( )) {
//        double strongestValid = 0.0;
//        int maxFitness = 0, maxFitID = 0;
//        bool validFound = false;
//        for(int i = 0; i < source.population_size; ++i) {
//            if(source.individuals[ i ]->isValid( ) && source.individuals[ i ]->getFitness( ) > maxFitness) {
//                validFound = true;
//                maxFitID = i;
//                maxFitness = source.individuals[ i ]->getFitness( );
//            }
//        }
//        if(maxFitness == 0) {
//            maxFitness = source.individuals[ maxFitID ]->getFitness( );
//        }
//        if(maxFitness < source.individuals[ source.strongestIndividualID ]->getFitness( )) {
//            os << "\n\n Strongest individual is the that was found.\n" << endl;
//        }
//        else {
//            strongestValid = ( maxFitness*100.0 / MAX_FITNESS*1.0 );
//            strongestValid = ( strongestValid < 0 ? strongestValid * -1.0 : strongestValid );
//            os << "\n\nStrongest Valid Individual: " << maxFitID << endl;
//            os << source.individuals[ maxFitID ]->printTable(source.timeSlots,
//                                                             source.timeslot_count);
//            os << "Fitnesss: " << strongestValid << "%" << endl;
//            os << "Valid: " << ( validFound ? "Yes" : "No" ) << endl;
//            os << "Professor Schedule: " << endl << source.individuals[ maxFitID ]->printProfTable( ) << endl;
//            improvement = ( strongestValid + abs(source.initFitness) ) *100.0 / abs(source.initFitness);
//
//            if(improvement < 0)
//                improvement *= -1.0;
//            os << "Improvement from Initial Schedule: " << improvement << "%" << endl;
//        }
//    }
//    else {
//        os << "Strongest was valid." << endl;
//    }
//
//    os << "Done." << endl;
//    return os;
//}
