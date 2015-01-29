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

Population::Population(string dataFilePath) :
    REPAIR_TRIES(50) {
    data_file_path = dataFilePath;
    population_size = 0;
    generation_count = 0;
    replacement_wait = 0;
    mutation_probability = 0;
    section_count = 0;
    professor_count = 0;
    weakestIndividualID = 0;
    professor_count = 0;
    credit_count = 0;
    timeslot_count = 0;
    suffix_cntr = 0;

    while (std::ifstream("stat" + to_string(suffix_cntr) + ".txt")) {
        suffix_cntr++;
    }

    string statFilePath = "stat" + to_string(suffix_cntr) + ".txt";
    string debugFilePath = "debug" + to_string(suffix_cntr) + ".txt";
    string outputFilePath = "output" + to_string(suffix_cntr) + ".txt";
    statFile.open(statFilePath.c_str(), ofstream::out);
    cout << "Stat file: " << statFilePath << endl;
    statFile << "Random Seed: " << h.seed << endl;
    debug.open(debugFilePath.c_str(), ofstream::out);
    cout << "Debug file: " << debugFilePath << endl;
    outputFile.open(outputFilePath.c_str(), ofstream::out);
    cout << "Output file: " << outputFilePath << endl;
    //Setup the Individuals array
    readDatFiles();


} //end Constructor

Population::~Population() {
    if (statFile.is_open())
        statFile.close();

    if (debug.is_open())
        debug.close();

    if (outputFile.is_open())
        outputFile.close();

    if (individuals) {
        for (int i = 0; i < population_size; ++i) {
            delete individuals[i];
        }
        delete[] individuals;
    }

    if (timeslotDaytime) {
        for (int i = 0; i < timeslot_count; ++i) {
            delete timeslotDaytime[i];
        }
        delete[] timeslotDaytime;
    }

    if (timeslotConflict) {
        for (int i = 0; i < timeslot_count; ++i) {
            delete timeslotConflict[i];
        }
        delete[] timeslotConflict;
    }

    if (timeslotConsecutive) {
        for (int i = 0; i < timeslot_count; ++i) {
            delete timeslotConsecutive[i];
        }
        delete[] timeslotConsecutive;
    }

    if (timeslotSpread) {
        for (int i = 0; i < timeslot_count; ++i) {
            delete timeslotSpread[i];
        }
        delete[] timeslotSpread;
    }

    delete[] sortedSectionList;
    delete[] timeCredits;

    if (incompatibleSectionsMatrix) {
        for (int i = 0; i < section_count; ++i) {
            delete[] incompatibleSectionsMatrix[i];
        } //end for
        delete[] incompatibleSectionsMatrix;
    }

    for (int i = 0; i < credit_count; ++i) {
        delete[] creditTimeSlot[i];
    }

    delete[] creditTimeSlot;

    for (int i = 0; i < section_count; ++i) {
        delete[] sectionProf[i];
    }

    delete[] sectionProf;

    delete[] sectionCredit;

    for (int i = 0; i < section_count; ++i) {
        delete[] sectionPref[i];
    }

    delete[] sectionPref;

    for (int i = 0; i < professor_count; ++i) {
        delete[] profPref[i];
    }

    delete[] profPref;

    for (int i = 0; i < professor_count; ++i) {
        delete[] profSection[i];
    }
    delete[] profSection;

    delete[] profCreditMax;

    delete[] timeCredLegend;

    for (int i = 0; i < professor_count; ++i) {
        for (int j = associatedProfessors[i][0][0] - 1; j >= 0; --j) {
            delete[] associatedProfessors[i][j];
        }
        delete[] associatedProfessors[i];
    }
    delete[] associatedProfessors;
} //end destructor

void Population::readDatFiles() {
    //vector<string> courses;

    ifstream inFile(data_file_path);

    cout << "Beginning Initial Setup...." << endl << endl;

    //Read the parameters
    readParameters(inFile);

    //Initialize the arrays
    incompatibleSectionsMatrix = new bool*[section_count]; //list of incompatible section for each section
    sectionCredit = new double[section_count]; //credit value of each section
    profCreditMax = new double[section_count]; //max credits for each professor
    sectionProf = new int*[section_count]; //list of professor for each section
    sectionTimeslot = new int*[section_count]; //list of timeslots for each section
    profSection = new int*[professor_count]; //list of section for each professor
    associatedProfessors = new int**[professor_count]; //list of professor associates
    sectionPref = new int*[section_count]; //list of preferences for each section
    profPref = new int*[professor_count];; //list of preferences for each prof
    timeCredLegend = new double[credit_count]; //credit value of each timeslot
    creditTimeSlot = new int*[credit_count];//list of timeslot for each credit

    sortedSectionList = new int[section_count];
    timeslotDaytime = new bool*[timeslot_count];
    timeslotConflict = new bool*[timeslot_count];
    timeslotConsecutive = new bool*[timeslot_count];
    timeslotSpread = new bool*[timeslot_count];
    timeCredits = new double[timeslot_count];

    //timeSlots = new TimeSlot*[ timeslot_count ]; //timeslot array
    individuals = new Chromosome*[population_size]; //Population array

    readSectionList(inFile); //Read section list
    readSectionCreditList(inFile); //Read section list
    readProfessorCreditList(inFile); //Read section list
    readSectionProfessorList(inFile); //Read section prof list
    readSectionTimeslotList(inFile); //Read section timeslot list
    readProfessorSectionList(inFile); //Read prof section list
    readAssociatedProfessorList(inFile); //Read associted prof list
    readCoursePref(inFile); //Read the course time preferences
    readProfPref(inFile); //Read professor time preferences
    readTimeCreditLegend(inFile);// Read time credit legend
    readCreditTimeslot(inFile); //Read the creditTimeslot array
    readTimeSlotList(inFile); //Read the timeslot list
    readInitialSchedule(inFile); //Read the initial schedule

    inFile.close();
    vector<int> tempSortedSectionList;
    for (int i = 0; i < section_count; i++) {
        tempSortedSectionList.push_back(i);
    }
    //sort(tempSortedSectionList.begin( ), tempSortedSectionList.end( ), bind(&Population::sortByProfessorCount, this, std::placeholders::_1, std::placeholders::_2));
    sort(tempSortedSectionList.begin(), tempSortedSectionList.end(), bind(&Population::sortByTimeslotCount, this, std::placeholders::_1, std::placeholders::_2));

    for (int i = 0; i < section_count; ++i) {
        sortedSectionList[i] = tempSortedSectionList.at(i);
    }

    tempSortedSectionList.clear();
    double courseSum = 0;
    //Sum total credits for courses
    for (int sec = 0; sec < section_count; ++sec) {
        courseSum += sectionCredit[sec];
    }

    //Sum total credits for prof
    double profSum = 0;
    for (int prof = 0; prof < professor_count; ++prof) {
        profSum += profCreditMax[prof];
    }

    statFile << "Course Credit Sum: " << courseSum << endl;
    statFile << "Prof Credit Sum: " << profSum << endl;


    /**** Setup soft constraints ****/
    cout << endl;
    weakestIndividualID = 0;

    prepareDataStatistics();

    //individualValidity[ 0 ] = individuals[ 0 ]->isValid();
    statFile << "First Valid: " << individuals[0]->isValid() << endl << endl;

    //Now we have enough basic information that we can populate our individuals based on the first one.
    initPopulationFromFirst();

    //Output all individuals
    if (DEBUG_INIT_POPULATION)
        for (int i = 0; i < population_size; ++i) {
            debug << "Individual " << i << endl;
            debug << individuals[i]->print();
        } //end for


    cout << endl << "Initial Setup Complete" << endl << endl;
    statFile << "Generation,";// Replaced, ";
    /*for(int x = 0; x < population_size; ++x) {
        statFile << to_string(x) << ",";
    }*/

    statFile << "Mean, SD;" << endl;

    statFile << "0," << GetFitnessData();
} //end readDatFiles()

bool Population::sortByProfessorCount(int i, int j) {
    return sectionProf[i][0] > sectionProf[j][0];
}

bool Population::sortByTimeslotCount(int i, int j) {
    int creditTimeRow_i = 0, creditTimeRow_j = 0;

    //Get the timeslot row for i
    while (creditTimeRow_i < credit_count && sectionCredit[i] != timeCredLegend[creditTimeRow_i])
        creditTimeRow_i++;
    //Get the timeslot row for j
    while (creditTimeRow_j < credit_count && sectionCredit[j] != timeCredLegend[creditTimeRow_j])
        creditTimeRow_j++;
    return creditTimeSlot[creditTimeRow_i][0] > creditTimeSlot[creditTimeRow_j][0];
}

void Population::prepareDataStatistics() {
    int min = section_count, max = 0, total = 0;
    double mean = 0.0;
    //Get min, max, mean incompatible associates per section
    for (int index = 0; index < section_count; ++index) {
        int currentIncomps = 0;
        for (int section = 0; section < section_count; section++) {
            if (incompatibleSectionsMatrix[index][section]) currentIncomps++;
        }
        total += currentIncomps;
        if (min > currentIncomps) {
            min = currentIncomps;
        }
        if (max < currentIncomps) {
            max = currentIncomps;
        }
    }
    mean = (double)total / (double)section_count;
    statFile << "Incompatible Sections: " << endl;
    statFile << "Range: [" << min << "," << max << "]" << endl;
    statFile << "Mean: " << fixed << setprecision(2) << mean << endl << endl;

    //Get min, max, mean professor per section
    min = professor_count;
    max = 0;
    total = 0;
    mean = 0.0;
    for (int index = 0; index < section_count; ++index) {
        total += sectionProf[index][0];
        if (min > sectionProf[index][0]) {
            min = sectionProf[index][0];
        }
        if (max < sectionProf[index][0]) {
            max = sectionProf[index][0];
        }
    }
    mean = (double)total / (double)section_count;
    statFile << "Professors Per Section: " << endl;
    statFile << "Range: [" << min << "," << max << "]" << endl;
    statFile << "Mean: " << fixed << setprecision(2) << mean << endl << endl;

    //Get min, max, mean associates per professor
    min = section_count;
    max = 0;
    total = 0;
    mean = 0.0;
    for (int index = 0; index < professor_count; ++index) {
        total += profSection[index][0];
        if (min > profSection[index][0]) {
            min = profSection[index][0];
        }
        if (max < profSection[index][0]) {
            max = profSection[index][0];
        }
    }
    mean = (double)total / (double)professor_count;
    statFile << "Sections Per Professor: " << endl;
    statFile << "Range: [" << min << "," << max << "]" << endl;
    statFile << "Mean: " << fixed << setprecision(2) << mean << endl << endl;

    //Get min, max, mean timeslots per credit
    min = timeslot_count;
    max = 0;
    total = 0;
    mean = 0.0;
    for (int index = 0; index < section_count; ++index) {
        int timeIndex = 0;

        while (timeCredLegend[timeIndex] != sectionCredit[index])
            timeIndex++;

        total += creditTimeSlot[timeIndex][0];
        if (min > creditTimeSlot[timeIndex][0]) {
            min = creditTimeSlot[timeIndex][0];
        }
        if (max < creditTimeSlot[timeIndex][0]) {
            max = creditTimeSlot[timeIndex][0];
        }
    }
    mean = (double)total / (double)credit_count;
    statFile << "Timeslots per Section: " << endl;
    statFile << "Range: [" << min << "," << max << "]" << endl;
    statFile << "Mean: " << fixed << setprecision(2) << mean << endl << endl;

    statFile << "Initial Stat Complete..." << endl << endl << endl;

}//end prepareDataStatistics()

void Population::readParameters(ifstream &inFile) {
    string currLine;
    if (inFile.is_open()) {
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
        section_count = stoi(currLine);

        getline(inFile, currLine);
        professor_count = stoi(currLine);

        getline(inFile, currLine);
        timeslot_count = stoi(currLine);

        getline(inFile, currLine);
        credit_count = stoi(currLine);

        getline(inFile, currLine);
        max_fitness = stoi(currLine);
        getline(inFile, currLine);
    }
}

/*
This method will read the section data file. It will then add the list of courses it finds to the vector.
*/
void Population::readSectionList(ifstream &inFile) {
    cout << "Starting reading sections..." << endl;

    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*SECTION*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int section = stoi(tokenizedVersion[0]);
            string incompBitString = tokenizedVersion[1];
            incompatibleSectionsMatrix[section] = new bool[section_count];
            for (int i = 0; i < section_count; i++) {
                if (incompBitString[i] == '1')
                    incompatibleSectionsMatrix[section][i] = true;
                else
                    incompatibleSectionsMatrix[section][i] = false;
            }
        } //end while
    } //end if

    statFile << "New Sections Added: " << section_count << endl;
} //end readSectionList

void Population::readSectionCreditList(ifstream &inFile) {
    cout << "Starting reading section credits..." << endl;
    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*SECTIONCREDIT*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int section = stoi(tokenizedVersion[0]);
            double creditValue = stod(tokenizedVersion[1]);
            sectionCredit[section] = creditValue;
        } //end while
    } //end if
} //end readSectionCreditList

void Population::readTimeCreditLegend(ifstream &inFile) {
    cout << "Starting reading time credit legend..." << endl;
    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*TIME*CREDIT*LEGEND*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int creditID = stoi(tokenizedVersion[0]);
            double creditValue = stod(tokenizedVersion[1]);
            timeCredLegend[creditID] = creditValue;
        } //end while
    } //end if
} //end readTimeCreditLegend

void Population::readProfessorCreditList(ifstream &inFile) {
    cout << "Starting reading professor credits..." << endl;
    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*PROFCREDIT*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int professor = stoi(tokenizedVersion[0]);
            double creditValue = stod(tokenizedVersion[1]);
            profCreditMax[professor] = creditValue;
        } //end while
    } //end if
} //end readProfessorCreditList

void Population::readCreditTimeslot(ifstream &inFile) {
    cout << "Starting reading professor per sections..." << endl;

    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*CREDIT*TIMESLOT*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int creditRow = stoi(tokenizedVersion[0]);
            int timeslots = stoi(tokenizedVersion[1]);
            creditTimeSlot[creditRow] = new int[timeslots + 1];
            creditTimeSlot[creditRow][0] = timeslots;
            if (timeslots > 0) {
                for (int x = 1; x < timeslots + 1; ++x) {
                    creditTimeSlot[creditRow][x] = stoi(tokenizedVersion[x + 1]);
                }
            }
        } //end while
    } //end if
} //end readCreditTimeslot

void Population::readSectionProfessorList(ifstream &inFile) {
    cout << "Starting reading professor per sections..." << endl;

    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*SECTION*PROF*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int section = stoi(tokenizedVersion[0]);
            int professors = stoi(tokenizedVersion[1]);
            sectionProf[section] = new int[professors + 1];
            sectionProf[section][0] = professors;
            if (professors > 0) {
                for (int x = 1; x < professors + 1; ++x) {
                    sectionProf[section][x] = stoi(tokenizedVersion[x + 1]);
                }
            }
        } //end while
        //cout << "Read list:" << endl;
    } //end if
} //end readSectionProfessorList

void Population::readSectionTimeslotList(ifstream &inFile) {
    cout << "Starting reading timeslots per sections..." << endl;

    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*SECTION*TIMESLOT*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int section = stoi(tokenizedVersion[0]);
            int timeslots = stoi(tokenizedVersion[1]);
            sectionTimeslot[section] = new int[timeslots + 1];
            sectionTimeslot[section][0] = timeslots;
            if (timeslots > 0) {
                for (int x = 1; x < timeslots + 1; ++x) {
                    sectionTimeslot[section][x] = stoi(tokenizedVersion[x + 1]);
                }
            }
        } //end while
          //cout << "Read list:" << endl;
    } //end if
} //end readSectionTimeslotList

void Population::readProfessorSectionList(ifstream &inFile) {
    cout << "Starting reading professor per sections..." << endl;

    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*PROF*SECTION*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int professor = stoi(tokenizedVersion[0]);
            int sections = stoi(tokenizedVersion[1]);

            profSection[professor] = new int[sections + 1];
            profSection[professor][0] = sections;
            if (sections > 0) {
                for (int x = 1; x < sections + 1; ++x) {
                    profSection[professor][x] = stoi(tokenizedVersion[x + 1]);
                }
            }
        } //end while
    } //end if
} //end readProfessorSectionList

void Population::readAssociatedProfessorList(ifstream &inFile) {
    cout << "Starting reading professor associate list..." << endl;

    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*PROF*ASSOCIATE*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int professor = stoi(tokenizedVersion[0]);
            int associates = stoi(tokenizedVersion[1]);

            associatedProfessors[professor] = new int*[associates + 1];
            associatedProfessors[professor][0] = new int[1];
            associatedProfessors[professor][0][0] = associates;
            if (associates > 0) {
                int currentLoc = 2;
                for (int x = 1; x < associates + 1; ++x) {
                    int associateID = stoi(tokenizedVersion[currentLoc++]);
                    int sectionsShared = stoi(tokenizedVersion[currentLoc++]);
                    associatedProfessors[professor][x] = new int[sectionsShared + 2];
                    associatedProfessors[professor][x][0] = associateID;
                    associatedProfessors[professor][x][1] = sectionsShared;
                    for (int j = 0; j < sectionsShared; j++) {
                        associatedProfessors[professor][x][j + 2] = stoi(tokenizedVersion[currentLoc++]);
                    }
                }
            }
        } //end while
    } //end if
}

void Population::readTimeSlotList(ifstream &inFile) {
    cout << "Starting reading TimeSlots" << endl;

    vector<string> tokenizedVersion;
    string currLine;

    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*TIMESLOT*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int timeslotID = stoi(tokenizedVersion.at(0));
            double creds = stod(tokenizedVersion.at(1));
            timeCredits[timeslotID] = creds;
            int isMorning = stoi(tokenizedVersion.at(2)), isAfternoon = stoi(tokenizedVersion.at(3)), isEvening = stoi(tokenizedVersion.at(4));
            string overlap = tokenizedVersion.at(5);
            string consecutive = tokenizedVersion.at(6);
            string spreadout = tokenizedVersion.at(7);

            timeslotDaytime[timeslotID] = new bool[3];
            if (isMorning == 0)
                timeslotDaytime[timeslotID][0] = false;
            else
                timeslotDaytime[timeslotID][0] = true;
            if (isAfternoon == 0)
                timeslotDaytime[timeslotID][1] = false;
            else
                timeslotDaytime[timeslotID][1] = true;
            if (isEvening == 0)
                timeslotDaytime[timeslotID][2] = false;
            else
                timeslotDaytime[timeslotID][2] = true;

            timeslotConflict[timeslotID] = new bool[timeslot_count];
            timeslotConsecutive[timeslotID] = new bool[timeslot_count];
            timeslotSpread[timeslotID] = new bool[timeslot_count];
            for (int i = 0; i < timeslot_count; ++i) {
                if (overlap[i] == '1')
                    timeslotConflict[timeslotID][i] = true;
                else
                    timeslotConflict[timeslotID][i] = false;
                if (consecutive[i] == '1')
                    timeslotConsecutive[timeslotID][i] = true;
                else
                    timeslotConsecutive[timeslotID][i] = false;
                if (spreadout[i] == '1')
                    timeslotSpread[timeslotID][i] = true;
                else
                    timeslotSpread[timeslotID][i] = false;
            }
        }
    }
} //end readTimeSlotList

void Population::readInitialSchedule(ifstream &inFile) {
    cout << "Initiating First Chromosome..." << endl;

    vector<string> tokenizedVersion;
    string currLine;
    individuals[0] = new Chromosome(section_count, professor_count, timeslot_count, profCreditMax);

    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*INITIAL*") == 0)
                break;

            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);

            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int geneLocation = stoi(tokenizedVersion.at(0));
            Gene current(stoi(tokenizedVersion.at(1)), stoi(tokenizedVersion.at(2)));
            individuals[0]->setGene(geneLocation, current);
            //individuals[ 0 ]->setProf(geneLocation, stoi(tokenizedVersion.at(1)), sectionCredit[ geneLocation ]);
        }
        individuals[0]->updateProfLoad(sectionCredit);
        if (DEBUG_INIT)
            debug << individuals[0]->print() << endl;
    }
    individuals[0]->updateFitness(incompatibleSectionsMatrix, sectionPref, profPref, timeslotDaytime, timeslotConflict, timeslotConsecutive, timeslotSpread);
    weakestIndividualID = 0;
    lowestFitnessSeen = individuals[0]->getFitness();
    strongestIndividualID = 0;
    highestFitnessSeen = individuals[0]->getFitness();

    statFile << "Prof Rating: " << endl << individuals[0]->getProfessorLoads() << endl;
    initFitness = individuals[0]->getFitness();
    statFile << "Initial Schedule Fitness: " << initFitness << endl;
    cout << "First Chromosome Initialized." << endl;
}

void Population::initPopulationFromFirst() {
    cout << "Initiating generation of population...." << endl;
    double original_mutation_probability = mutation_probability;
    mutation_probability = 0.1;
    int len = to_string(population_size).length();
    cout << "Generating individual : ";

    for (int i = 1; i < population_size; ++i) {

        if (i == 0) {
            cout << setw(len) << "0";
        }
        else {
            for (int b = 0; b < len; ++b) {
                cout << '\b';
            }
            cout << setw(len) << to_string(i + 1);
        }
        individuals[i] = new Chromosome(individuals[0]);
        if (DEBUG_INIT_POPULATION_COMPARED)
            debug << "Pre-Mutation: " << endl << individuals[i]->print();

        individuals[i]->evolve(sortedSectionList, sectionProf, sectionTimeslot, &h, mutation_probability, sectionCredit, incompatibleSectionsMatrix, timeslotConflict, credit_count, profSection, associatedProfessors, sectionPref, profPref, timeslotDaytime, timeslotConsecutive, timeslotSpread);

        if (individuals[i]->getFitness() < individuals[weakestIndividualID]->getFitness()) {
            weakestIndividualID = i;
            lowestFitnessSeen = individuals[i]->getFitness();
        }

        if (individuals[i]->getFitness() > individuals[strongestIndividualID]->getFitness()) {
            strongestIndividualID = i;
            highestFitnessSeen = individuals[i]->getFitness();
        }
    }

    mutation_probability = original_mutation_probability;

    cout << endl << "Population generated." << endl;
}

void Population::readCoursePref(ifstream &inFile) {
    string currLine;
    //1. Create a temp coursePref list that will be used to populate section pref
    //2. populate the coursePref list using a similar approach as to populating course data
    //3. populate sectionPref from the coursePref

    vector<string> tokenizedVersion;
    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*COURSEPREF*") == 0)
                break;

            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int sectionID = stoi(tokenizedVersion.at(0));
            sectionPref[sectionID] = new int[3];
            for (int idx = 0; idx < 3; ++idx) {
                sectionPref[sectionID][idx] = stoi(tokenizedVersion.at(idx + 1));
            } //end for idx
        } //while(getline(inFile, currLine))
    } //if (inFile.is_open( ))
}

void Population::readProfPref(ifstream &inFile) {
    string currLine;
    vector<string> tokenizedVersion;

    if (inFile.is_open()) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*PROFPREF*") == 0)
                break;

            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);

            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int profIndex = stoi(tokenizedVersion.at(0));
            profPref[profIndex] = new int[3];
            for (int idx = 0; idx < 3; ++idx) {
                profPref[profIndex][idx] = stoi(tokenizedVersion.at(idx + 1));
            } //end for
        } //end while (getline(inFile, currLine))
    } //end if(inFile.is_open( ))
} //end readProfPref

int Population::getWeightedRandomIndividual() {
    int minWeight = max_fitness;
    int maxWeight = 0;
    for (int i = 0; i < population_size; i++) {
        maxWeight += individuals[i]->getFitness();
        if (maxWeight > (int)pow(2, 30))
        {
            maxWeight = -1;
            break;
        }
        if (individuals[i]->getFitness() < minWeight) {
            minWeight = individuals[i]->getFitness();
        }
    }

    if (maxWeight > 0) {
        int randomWeight = h.randNum(minWeight, maxWeight);
        int sacrifice = 0;
        for (sacrifice = 0; sacrifice < population_size; ++sacrifice) {
            maxWeight -= individuals[sacrifice]->getFitness();
            if (maxWeight <= 0)
                break;
        }
        return sacrifice;
    }
    else {
        return h.randNum(0, population_size - 1);
    }
} //end getWeightedRandomIndividual

bool Population::allValid() {
    for (int i = 0; i < population_size; ++i) {
        if (!individuals[i]->isValid()) {
            return false;
        }
    }
    return true;
} //end allValid

bool Population::allValid(int currentGeneration) {
    bool val = allValid();
    /*if(val) {
        cout << "Valid population at " << currentGeneration << endl;
    }*/

    return val;
} //end allValid(int currentGeneration)

void Population::Evolve() {

    int currentGeneration = 1;
    int threshold_generation = (int)((double)generation_count * 0.25);
    cout << "Total generations to evolve over: " << generation_count << endl;
    cout << "Threshold for invalid individuals " << threshold_generation << endl;
    cout << "Current Generation: ";
    int len = to_string(generation_count).length();

    int generationOfFullValidity = generation_count + 1;
    int generationLoop = 0;
    int dots = 0;

    
    for (currentGeneration = 0; currentGeneration <= generation_count /*&& generationLoop < replacement_wait*/; ++currentGeneration) {
        
        if (currentGeneration == 0) {
            cout << setw(len) << "0";
        }
        else if (currentGeneration % 100 == 0 || currentGeneration < 100) {
            for (int b = 0; b < (len + dots); ++b) {
                cout << '\b';
            }
            cout << setw(len) << to_string(currentGeneration);
            if (dots > 0) {
                cout << setw(dots) << " ";
                for (int b = 0; b < dots; ++b) {
                    cout << '\b';
                }
            }
            dots = 0;
        }
        else if (currentGeneration % 10 == 0) {
            cout << '.';
            dots++;
        }

        int parentID;// = getWeightedRandomIndividual( );
        parentID = h.randNum(0, population_size - 1);
        if (individuals[parentID]->getFitness() < lowestFitnessSeen)
            lowestFitnessSeen = individuals[parentID]->getFitness();
        if (individuals[parentID]->getFitness() > highestFitnessSeen)
            highestFitnessSeen = individuals[parentID]->getFitness();
        Chromosome * child = new Chromosome(individuals[parentID]);

        if (DEBUG_EVOLVE) {
            debug << "Sacrifice source: " << parentID << endl;
            debug << "Sacrifice: " << endl << child->print() << "Source: " << endl << individuals[parentID]->print() << endl;
        }

        child->evolve(sortedSectionList, sectionProf, sectionTimeslot, &h, mutation_probability, sectionCredit, incompatibleSectionsMatrix, timeslotConflict, credit_count, profSection, associatedProfessors, sectionPref, profPref, timeslotDaytime, timeslotConsecutive, timeslotSpread);
        int diffCount = child->DifferenceCount(individuals[parentID]);
        if (currentGeneration > threshold_generation && !child->isValid()) {
            generationLoop++;
            currentGeneration--;
            delete child;
            continue;
        }
        if (child->getFitness() > individuals[parentID]->getFitness()) {
            generationLoop = 0;
            delete individuals[parentID];
            individuals[parentID] = new Chromosome(child);
        }
        else if (child->getFitness() > individuals[weakestIndividualID]->getFitness()) {
            generationLoop = 0;
            //Only optimize if the sacrifice is valid.
            if (DEBUG_EVOLVE) {
                debug << "Replacing " << weakestIndividualID << " with the sacrifice." << endl;
                debug << "Original\t\tNew" << endl;
                for (int indexB = 0; indexB < section_count; ++indexB)
                    debug << individuals[weakestIndividualID]->print(indexB)
                    << "  ->  " << child->print(indexB) << endl;
                debug << "New weakest: " << endl;
            }

            delete individuals[weakestIndividualID];
            individuals[weakestIndividualID] = new Chromosome(child);

            if (DEBUG_EVOLVE)
                debug << individuals[weakestIndividualID]->print();
            bool valid_population = allValid();
            //If we are past the threshold generation, we want to start creating a valid population.
            if (currentGeneration > threshold_generation && !valid_population) {
                int weakestInvalid = -1;
                int lowestFitness = lowestFitnessSeen;
                for (int j = 0; j < population_size; ++j) {
                    if (!individuals[j]->isValid() && individuals[j]->getFitness() < lowestFitness) {
                        weakestInvalid = j;
                        lowestFitness = individuals[j]->getFitness();
                    }
                    if (individuals[j]->getFitness() > individuals[strongestIndividualID]->getFitness()) {
                        strongestIndividualID = j;
                    }
                }
                if (weakestInvalid != -1)
                    weakestIndividualID = weakestInvalid;
                else
                    goto alternateWeakestSelection;
            }
            else {
            alternateWeakestSelection:
                for (int j = 0; j < population_size; ++j) {
                    if (individuals[j]->getFitness() < individuals[weakestIndividualID]->getFitness()) {
                        weakestIndividualID = j;
                    }
                    if (individuals[j]->getFitness() > individuals[strongestIndividualID]->getFitness()) {
                        strongestIndividualID = j;
                    }
                }
            }
        }
        if (allValid(currentGeneration) && generationOfFullValidity > currentGeneration && currentGeneration > threshold_generation) {
            generationOfFullValidity = currentGeneration;
        }

        if (DEBUG_EVOLVE) {
            debug << "Current State" << endl;
            for (int indexC = 0; indexC < population_size; ++indexC) {
                debug << "Individual " << indexC << endl;
                debug << individuals[indexC]->print();
            }
            debug << "-----------------------------------------------------------" << endl;
        }
        

        if (currentGeneration > 0) {
            statFile << currentGeneration << ",";
            statFile << GetFitnessData();
            debug << currentGeneration << ",\t" << diffCount << ",\t" << (diffCount*1.0 / (section_count*2.0));
            if (child->isValid()) {
                debug << ", 1" << endl;
            }
            else {
                debug << ", 0" << endl;
            }
        }
        delete child;
    }
    cout << endl << "Evolution Complete." << endl;
    statFile << currentGeneration << ",";
    statFile << GetFitnessData() << endl;
    statFile << "Population valid at: " << generationOfFullValidity << endl;
}

string Population::PrintTableFormat() {
    string rtnVal = "";

    if (DEBUG_PRINTTABLE) {
        for (int i = 0; i < 1; ++i) {
            //rtnVal += individuals[ i ]->printTable(timeSlots, timeslot_count, sectionCredit);
            rtnVal += "\n\n";
        }
    }
    else {
        for (int i = 0; i < population_size; ++i) {
            // rtnVal += individuals[ i ]->printTable(timeSlots, timeslot_count);
            rtnVal += "\n\n";
        }
    }

    return rtnVal;
}

string Population::GetFitnessData() {
    int * allFitness = new int[population_size - 2];
    stringstream s;
    for (int i = 0; i < population_size; ++i) {
        allFitness[i - 1] = individuals[i]->getFitness();
        s << individuals[ i ]->getFitness( ) << ",";
    }
    double mean = Utility::CalculateMean(allFitness, population_size - 1);
    double sd = Utility::StandardDeviation(allFitness, population_size - 1, mean);
    s << mean << "," << sd;
    s << "\n";
    return s.str();
}

void Population::PrintEnd() {
    outputFile << "Final Population:" << endl;
    double strongestFitness = individuals[strongestIndividualID]->getFitness();

    if (PRINT_WHOLE_POPULATION) {
        outputFile << "Printing Current Population..." << endl;
        int* fitnessValues = new int[population_size];
        for (int i = 0; i < population_size; ++i) {
            outputFile << "Individual " << i << endl;
            fitnessValues[i] = individuals[i]->getFitness();
            outputFile << "Fitnesss: " << fitnessValues[i] << endl;
            outputFile << "Valid: "
                << (individuals[i]->isValid() ?
                "Yes" : "No") << endl;
            outputFile << "*******************************************************"
                << endl << endl;
        }
        outputFile << "Current fitness values: " << endl;
        for (int i = 0; i < population_size - 1; ++i) {
            outputFile << fitnessValues[i] << ",";
        }
        outputFile << fitnessValues[population_size - 1] << endl;
    } //if (PRINT_WHOLE_POPULATION)

    outputFile << "\n\nStrongest Individual: " << strongestIndividualID << endl;
    outputFile << "**BEGINRESULT**" << endl;
    outputFile << individuals[strongestIndividualID]->printTuple();
    outputFile << "**ENDRESULT**" << endl;
    outputFile << "Fitness: " << strongestFitness << endl;

    outputFile << "Valid: " << (individuals[strongestIndividualID]->isValid(incompatibleSectionsMatrix, timeslotConflict, true) ? "Yes" : "No") << endl;

    outputFile << "Professor Schedule: " << endl << individuals[strongestIndividualID]->printProfTable() << endl;

    if (!individuals[strongestIndividualID]->isValid()) {
        double strongestValid = 0.0;
        int maxFitness = 0, maxFitID = 0;
        bool validFound = false;
        for (int i = 0; i < population_size; ++i) {
            if (individuals[i]->isValid() && individuals[i]->getFitness() > maxFitness) {
                validFound = true;
                maxFitID = i;
                maxFitness = individuals[i]->getFitness();
            }
        }
        if (maxFitness == 0) {
            maxFitness = individuals[maxFitID]->getFitness();
        }
        if (maxFitness < individuals[strongestIndividualID]->getFitness()) {
            outputFile << "\n\n No strong valid solution found.\n" << endl;
        }
        else {
            strongestValid = maxFitness;
            outputFile << "\n\nStrongest Valid Individual: " << maxFitID << endl;
            outputFile << "Fitnesss: " << strongestValid << endl;
            outputFile << "Valid: " << (validFound ? "Yes" : "No") << endl;
            outputFile << "Professor Schedule: " << endl << individuals[maxFitID]->printProfTable() << endl;
        }
    }
    else {
        outputFile << "Strongest was valid." << endl;
    }
    int cntr = 1;
    outputFile << "*START*PRINTING*OTHER*VALID*" << endl;
    for (int i = 0; i < population_size; ++i) {
        bool unique = true;
        for (int j = 0; j < i; ++j) {
            if (individuals[i]->equals(individuals[j])) {
                unique = false;
            }
        }
        if (!unique && individuals[i]->isValid(incompatibleSectionsMatrix, timeslotConflict, false)) {
            outputFile << "*START*SCHEDULE*" << cntr << endl;
            outputFile << individuals[i]->printTuple() << endl;
            outputFile << "Validity: " << (individuals[i]->isValid() ? "Yes" : "No") << endl;
            outputFile << "Fitness: " << individuals[i]->getFitness() << endl;
            outputFile << "*END*SCHEDULE*" << endl;
            cntr++;
        }
    }
    outputFile << "*END*PRINTING*OTHER*VALID*" << endl;
    outputFile << "Unique Solutions: " << --cntr << endl;
    outputFile << "Done." << endl;
    statFile.close();
    //outputFile.close( );
}
