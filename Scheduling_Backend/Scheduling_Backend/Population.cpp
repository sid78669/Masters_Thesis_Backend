#include "Population.h"


Population::Population(string dataFilePath, int populationSize, int generationCount, double mutationProbability) {
    data_file_path = dataFilePath;
    population_size = populationSize;
    generation_count = generationCount;
    mutation_probability = mutationProbability;
    section_count = 0;
    professorCount = 0;
    readDatFiles( );
    //exit(0);
    for (int i = 1; i < populationSize; i++) {
        individuals[i] = new Chromosome(section_count);
    }

}


Population::~Population( ) {
    delete[] individuals;
    delete[] timeSlots;
    for (int i = 0; i < section_count; i++) {
        delete[] incompatibleSections[i];
    }
    delete[] incompatibleSections;
    for (int i = 0; i < courseCount; i++) {
        delete[] courseSection[i];
    }

    delete[] courseSection;
}

void Population::readDatFiles( ) {
    vector<string> courses;
    int courseCount = 1000;


    cout << "Beginning Initial Setup...." << endl;

    //Read the section List
    readSectionList(courses);
    //Since we have the section count now, initialize all arrays dependent on the number of sections.
    incompatibleSections = new int*[section_count];
    sectionProf = new int*[section_count];
    sectionCredit = new double[section_count];

    //Read the course list
    readCourseList(courses);

    //Now that the courses are setup, time to setup the professor. This is done AFTER courses because each professor is associated with a course.
    readProfessorList(courses);

    //The only thing left for initial data, other than an initial chromosome, is timeslots.
    readTimeSlotList( );

    //Setup the Individuals array
    individuals = new Chromosome*[population_size];

    //FINALLY! Use the given chromosome blueprint to create the first chromosome
    readInitialSchedule( );

    //Now we have enough basic information that we can populate our individuals based on the first one.
    initPopulationFromFirst( );

    cout << "Initial Setup Complete" << endl;
}

/*
This method will read the section data file. It will then add the list of courses it finds to the vector.
*/
void Population::readSectionList(vector<string> &courseList) {
    cout << "Starting reading sections..." << endl;
    int courseCount = 0;
    int ** tempCourseSections = new int*[ARRAY_MAX];

    ifstream inFile(data_file_path + "/sections.dat");
    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            courseList.push_back(tokenizedVersion[0]);
            int sections = stoi(tokenizedVersion[1]);
            tempCourseSections[courseList.size( ) - 1] = new int[sections + 1];
            tempCourseSections[courseList.size( ) - 1][0] = sections;
            for (int i = 1; i <= sections; i++) {
                tempCourseSections[courseList.size( ) - 1][i] = section_count++;
            }

            if (DEBUG_SECTION)
                cout << currLine << "> " << tokenizedVersion[0] << "#" << tokenizedVersion[1] << "** " << endl;
        }
    }

    //Initialize the courseSection array and populate it.
    courseSection = new int*[courseList.size( )];
    for (int i = 0; i < (signed) courseList.size( ); i++) {
        courseSection[i] = new int[tempCourseSections[i][0] + 1];
        if (DEBUG_SECTION)
            cout << courseList.at(i) << ">> ";
        for (int j = 0; j <= tempCourseSections[i][0]; j++) {
            courseSection[i][j] = tempCourseSections[i][j];
            if (DEBUG_SECTION)
                cout << courseSection[i][j] << "_";
        }

        //Delete the temp sub array for that index.
        delete[] tempCourseSections[i];
        if (DEBUG_SECTION)
            cout << endl;
    }

    //delete the temp helper array
    delete[] tempCourseSections;
    courseCount = courseList.size( );
    cout << "New Courses Added: " << courseCount << endl;
}

/*
    This method will readh the course data file. It will then populate the incompatible section array and the sectionCredit arrays.
    */
void Population::readCourseList(vector<string> courseList) {
    cout << "Starting reading courses..." << endl;
    ifstream inFile(data_file_path + "/courses.dat");
    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');

            //Find the courseID
            int courseLoc = 0;
            while (courseList.at(courseLoc).compare(tokenizedVersion.at(0)) != 0)
                courseLoc++;


            //Setup data that will be same between related sections, i.e. credits and incompatible sections.
            double cred = stod(tokenizedVersion.at(1));
            vector<int> incompatibles;
            //Check if there are any incompatible sections.
            if (tokenizedVersion.size( ) > 2) {
                for (int inc = 2; inc < (signed) tokenizedVersion.size( ); inc++) {
                    //Find the courseID
                    int currIncomp = 0;
                    while (courseList.at(currIncomp).compare(tokenizedVersion.at(inc)) != 0)
                        currIncomp++;

                    //Add the sections for this course to the vector
                    for (int secIdx = 1; secIdx <= courseSection[currIncomp][0]; secIdx++) {
                        incompatibles.push_back(courseSection[currIncomp][secIdx]);
                    }
                }

            }

            if (DEBUG_COURSES) {
                for (auto inco : incompatibles)
                    cout << inco << ", ";
                cout << endl;
            }

            //Loop through the sections and set the values for credits and incompatible sections
            for (int i = 1; i <= courseSection[courseLoc][0]; i++) {
                int sectionID = courseSection[courseLoc][i];
                sectionCredit[sectionID] = cred;

                //Copy all the incompatible sections over.
                incompatibleSections[sectionID] = new int[incompatibles.size( ) + 1];
                incompatibleSections[sectionID][0] = incompatibles.size( );
                for (int j = 1; j <= (signed) incompatibles.size( ); j++) {
                    incompatibleSections[sectionID][j] = incompatibles.at(j - 1);
                }
            }
        }
    }


    cout << "Finished reading courses..." << endl;
}

void Population::readProfessorList(vector<string> courseList) {
    cout << "Starting reading professors...." << endl;

    ifstream inFile(data_file_path + "/professors.dat");
    string currLine;
    vector<int> * secProfs = new vector<int>[section_count];
    vector<string> tokenizedVersion;
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            for (int courseLoc = 1; courseLoc < (signed) tokenizedVersion.size( ); courseLoc++) {
                int courseID = 0;
                while (courseList.at(courseID).compare(tokenizedVersion.at(courseLoc)) != 0)
                    courseID++;
                for (int sects = 1; sects <= courseSection[courseID][0]; sects++) {
                    secProfs[courseSection[courseID][sects]].push_back(professorCount);
                }
            }
            professorCount++;
        }
        for (int i = 0; i < section_count && DEBUG_PROF; i++) {
            cout << i << "---" << endl;
            for (auto sp : secProfs[i])
                cout << sp << ", ";
            cout << endl;
        }
    }

    delete[] secProfs;

    cout << "Number of Professors added: " << professorCount << endl;
}

void Population::readTimeSlotList( ) {
    cout << "Starting reading TimeSlots" << endl;
    vector<vector<int>> cTimeSlot;

    vector<string> tokenizedVersion;
    string currLine;

    ifstream inFile(data_file_path + "/timeslot.dat");
    TimeSlot ** tempArray = new TimeSlot*[ARRAY_MAX];
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;

            Utility::CleanWhiteSpace(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            double creds = stod(tokenizedVersion.at(1));
            tempArray[timeSlotCount] = new TimeSlot(creds);
            for (int i = 0; i < 6; i++)
                tempArray[timeSlotCount]->setPeriod(i, tokenizedVersion.at(2 + i));
            int creditLoc = 0;
            while (!cTimeSlot.empty( ) && creditLoc < cTimeSlot.size( ) && tempArray[cTimeSlot.at(creditLoc).at(0)]->getCredits( ) != creds)
                creditLoc++;
            if (creditLoc >= cTimeSlot.size( )) {
                vector<int> newVec;
                cTimeSlot.push_back(newVec);
            }
            cTimeSlot.at(creditLoc).push_back(timeSlotCount);
            timeSlotCount++;
        }

        timeSlots = new TimeSlot*[timeSlotCount];
        for (int i = 0; i < timeSlotCount; i++) {
            timeSlots[i] = new TimeSlot((*tempArray[i]));
            delete tempArray[i];
            if (DEBUG_TIMESLOT)
                cout << i << " - " << timeSlots[i]->print( ) << endl;
        }

        creditCount = cTimeSlot.size( );
        creditTimeSlot = new int*[creditCount];
        for (int i = 0; i < creditCount; i++) {
            creditTimeSlot[i] = new int[cTimeSlot.at(i).size( ) + 1];
            creditTimeSlot[i][0] = cTimeSlot.at(i).size( );
            for (int j = 1; j <= creditTimeSlot[i][0]; j++) {
                creditTimeSlot[i][j] = cTimeSlot.at(i).at(j - 1);
            }
        }
    }

    delete[] tempArray;


    cout << "New TimeSlots Added: " << timeSlotCount << endl;
}
void Population::readInitialSchedule( ) {
    cout << "Initiating First Chromosome..." << endl;

    ifstream inFile(data_file_path + "/init.dat");
    vector<string> tokenizedVersion;
    string currLine;
    individuals[0] = new Chromosome(section_count);
    if (DEBUG_INIT_CHROMOSOME)
        cout << individuals[0]->print( ) << endl;
    int geneLocation = 0;
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;

            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');

            individuals[0]->setProf(geneLocation, stoi(tokenizedVersion.at(1)));
            individuals[0]->setTime(geneLocation, stoi(tokenizedVersion.at(2)));
            geneLocation++;
        }
        if (DEBUG_INIT_CHROMOSOME)
            cout << individuals[0]->print( ) << endl;
    }

    cout << "First Chromosome Initialized..." << endl;
}

void Population::initPopulationFromFirst( ) {

}