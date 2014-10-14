#include "Population.h"

Population::Population(string dataFilePath, int populationSize, int generationCount, int replacementWait, double mutationProbability) {
    data_file_path = dataFilePath;
    population_size = populationSize;
    generation_count = generationCount;
    replacement_wait = replacementWait;
    mutation_probability = mutationProbability;
    section_count = 0;
    professor_count = 0;
    weakestIndividualID = 0;
    readDatFiles( );
    
}//end Constructor


Population::~Population( ) {
    delete[] individuals;
    delete[] timeSlots;
    for (int i = 0; i < section_count; ++i) {
        delete[] incompatibleSections[i];
    }//end for
    delete[] incompatibleSections;
    for (int i = 0; i < course_count; ++i) {
        delete[] courseSection[i];
    }//end for

    delete[] courseSection;
}//end destructor

void Population::readDatFiles( ) {
    vector<string> courses;
    int course_count = 1000;

    ifstream inFile(data_file_path);

    cout << "Beginning Initial Setup...." << endl << endl;

    //Read the section List
    readSectionList(inFile, courses);
    cout << endl;
    //Since we have the section count now, initialize all arrays dependent on the number of sections.
    incompatibleSections = new int*[section_count];
    sectionProf = new int*[section_count];
    sectionCredit = new double[section_count];
    sectionPref = new int*[section_count];

    //Read the course list
    readCourseList(inFile, courses);
    cout << endl;

    //Now that the courses are setup, time to setup the professor. This is done AFTER courses because each professor is associated with a course.
    readProfessorList(inFile, courses);
    cout << endl;
    profPref = new int*[professor_count];

    //The only thing left for initial data, other than an initial chromosome, is timeslots.
    readTimeSlotList(inFile);
    cout << endl;

    //Setup the Individuals array
    individuals = new Chromosome*[population_size];
    individualValidity = new bool[population_size];

    /**** Setup soft constraints ****/
    //Read the course time preferences
    readCoursePref(inFile, courses);

    //Read the prof time preferences
    readProfPref(inFile);

    //FINALLY! Use the given chromosome blueprint to create the first chromosome
    readInitialSchedule(inFile);
    cout << endl;

    individualValidity[0] = validateChromosome(individuals[0]);
    cout << "First Valid: " << individualValidity[0] << endl << endl;

    //Now we have enough basic information that we can populate our individuals based on the first one.
    initPopulationFromFirst( );

    //Output all individuals
    if (DEBUG_INIT)
        for (int i = 0; i < population_size; ++i) {
        cout << "Individual " << i << endl;
        cout << individuals[i]->print( );
        }//end for


    inFile.close( );
    cout << endl << "Initial Setup Complete" << endl << endl;
}//end readDatFiles()

/*
This method will read the section data file. It will then add the list of courses it finds to the vector.
*/
void Population::readSectionList(ifstream &inFile, vector<string> &courseList) {
    cout << "Starting reading sections..." << endl;
    int course_count = 0;
    int * tempCourseSections = new int[ARRAY_MAX];

    //ifstream inFile(data_file_path + "/sections.dat");
    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*SECTION*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            courseList.push_back(tokenizedVersion[0]);
            int sections = stoi(tokenizedVersion[1]);
            tempCourseSections[courseList.size( ) - 1] = sections;

            if (DEBUG_SECTION)
                cout << currLine << "> " << tokenizedVersion[0] << "#" << tokenizedVersion[1] << "** " << endl;
        }//end while
    }//end if

    //Initialize the courseSection array and populate it.
    courseSection = new int*[courseList.size( )];
    for (int i = 0; i < (signed) courseList.size( ); ++i) {
        courseSection[i] = new int[tempCourseSections[i] + 1];
        courseSection[i][0] = tempCourseSections[i];
        if (DEBUG_SECTION)
            cout << courseList.at(i) << ">> (" << courseSection[i][0] << ") ";
        for (int j = 1; j <= courseSection[i][0]; ++j) {
            courseSection[i][j] = section_count++;
            if (DEBUG_SECTION)
                cout << courseSection[i][j] << "_";
        }//end for

        //Delete the temp sub array for that index.
        if (DEBUG_SECTION)
            cout << endl;
    }//end for

    //delete the temp helper array
    delete[] tempCourseSections;
    course_count = courseList.size( );
    cout << "New Courses Added: " << course_count << endl << "New Sections Added: " << section_count << endl;
}//end readSectionList

/*
    This method will readh the course data file. It will then populate the incompatible section array and the sectionCredit arrays.
    */
void Population::readCourseList(ifstream &inFile, vector<string> courseList) {
    cout << "Starting reading courses..." << endl;
    //ifstream inFile(data_file_path + "/courses.dat");
    string currLine;
    vector<string> tokenizedVersion;
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*COURSE*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');

            //Find the courseID
            /*int courseLoc = 0;
            while (courseList.at(courseLoc).compare(tokenizedVersion.at(0)) != 0)
            courseLoc++;*/
            int courseLoc = find(courseList.begin( ), courseList.end( ), tokenizedVersion.at(0)) - courseList.begin( );


            //Setup data that will be same between related sections, i.e. credits and incompatible sections.
            double cred = stod(tokenizedVersion.at(1));
            vector<int> incompatibles;
            //Check if there are any incompatible sections.
            if (tokenizedVersion.size( ) > 2) {
                for (int inc = 2; inc < (signed) tokenizedVersion.size( ); ++inc) {
                    //Find the courseID
                    /*int currIncomp = 0;
                    while (courseList.at(currIncomp).compare(tokenizedVersion.at(inc)) != 0)
                    currIncomp++;*/
                    int currIncomp = find(courseList.begin( ), courseList.end( ), tokenizedVersion.at(0)) - courseList.begin( );

                    //Add the sections for this course to the vector
                    for (int secIdx = 1; secIdx <= courseSection[currIncomp][0]; ++secIdx) {
                        incompatibles.push_back(courseSection[currIncomp][secIdx]);
                    }//end for
                }//end for
            }//end if

            if (DEBUG_COURSES) {
                for (auto inco : incompatibles)
                    cout << inco << ", ";
                cout << endl;
            }//end if

            //Loop through the sections and set the values for credits and incompatible sections
            for (int i = 1; i <= courseSection[courseLoc][0]; ++i) {
                if (DEBUG_COURSES)
                    cout << courseSection[courseLoc][i] << ": ";
                int sectionID = courseSection[courseLoc][i];
                sectionCredit[sectionID] = cred;

                //Copy all the incompatible sections over.
                incompatibleSections[sectionID] = new int[incompatibles.size( ) + 1];
                incompatibleSections[sectionID][0] = incompatibles.size( );
                for (int j = 1; j <= (signed) incompatibles.size( ); ++j) {
                    if (DEBUG_COURSES)
                        cout << ", " << incompatibles.at(j - 1);
                    incompatibleSections[sectionID][j] = incompatibles.at(j - 1);
                }//end for
                if (DEBUG_COURSES)
                    cout << endl;
            }//end for
        }//end while
    }//end if

    cout << "Finished reading courses. Updated " << course_count << " new courses." << endl;
}//end readCourseList

void Population::readProfessorList(ifstream &inFile, vector<string> courseList) {
    cout << "Starting reading professors...." << endl;

    string currLine;
    vector<int> * secProfs = new vector<int>[section_count];
    vector<string> tokenizedVersion;
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*PROFESSOR*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            for (int courseLoc = 1; courseLoc < (signed) tokenizedVersion.size( ); ++courseLoc) {
                int courseID = find(courseList.begin( ), courseList.end( ), tokenizedVersion.at(courseLoc)) - courseList.begin( );
                if (DEBUG_PROF)
                    cout << "Course: " << tokenizedVersion.at(courseLoc) << " ID: " << courseID << endl;
                for (int sects = 1; sects <= courseSection[courseID][0]; ++sects) {
                    secProfs[courseSection[courseID][sects]].push_back(professor_count);
                    if (DEBUG_PROF)
                        cout << "Section: " << courseSection[courseID][sects] << " Prof: " << professor_count << endl;
                }
            }
            professor_count++;
        }
        for (int i = 0; i < section_count; ++i) {
            if (DEBUG_PROF)
                cout << i << "---" << endl;
            sectionProf[i] = new int[secProfs[i].size( ) + 1];
            sectionProf[i][0] = secProfs[i].size( );
            int cntr = 1;
            for (auto sp : secProfs[i]) {
                sectionProf[i][cntr++] = sp;
                if (DEBUG_PROF)
                    cout << sp << ", ";
            }
            if (DEBUG_PROF)
                cout << endl;
        }
    }

    delete[] secProfs;

    cout << "Number of Professors added: " << professor_count << endl;
}

void Population::readTimeSlotList(ifstream &inFile) {
    cout << "Starting reading TimeSlots" << endl;
    vector<vector<int>> cTimeSlot;

    vector<string> tokenizedVersion;
    string currLine;

    TimeSlot ** tempArray = new TimeSlot*[ARRAY_MAX];
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*TIMESLOT*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            double creds = stod(tokenizedVersion.at(1));
            tempArray[timeslot_count] = new TimeSlot(creds);
            for (int i = 0; i < 6; ++i)
                tempArray[timeslot_count]->setPeriod(i, tokenizedVersion.at(2 + i));
            int creditLoc = 0;
            while (!cTimeSlot.empty( ) && creditLoc < (signed) cTimeSlot.size( ) && tempArray[cTimeSlot.at(creditLoc).at(0)]->getCredits( ) != creds)
                creditLoc++;
            if (creditLoc >= (signed) cTimeSlot.size( )) {
                vector<int> newVec;
                cTimeSlot.push_back(newVec);
            }
            cTimeSlot.at(creditLoc).push_back(timeslot_count);
            timeslot_count++;
        }

        timeSlots = new TimeSlot*[timeslot_count];
        for (int i = 0; i < timeslot_count; ++i) {
            timeSlots[i] = new TimeSlot((*tempArray[i]));
            delete tempArray[i];
            if (DEBUG_TIMESLOT)
                cout << i << " - " << timeSlots[i]->print( ) << endl;
        }

        credit_count = cTimeSlot.size( );
        creditTimeSlot = new int*[credit_count];
        for (int i = 0; i < credit_count; ++i) {
            creditTimeSlot[i] = new int[cTimeSlot.at(i).size( ) + 1];
            creditTimeSlot[i][0] = cTimeSlot.at(i).size( );
            for (int j = 1; j <= creditTimeSlot[i][0]; ++j) {
                creditTimeSlot[i][j] = cTimeSlot.at(i).at(j - 1);
            }
        }
    }

    delete[] tempArray;


    cout << "New TimeSlots Added: " << timeslot_count << endl;
}

void Population::readInitialSchedule(ifstream &inFile) {
    cout << "Initiating First Chromosome..." << endl;

    vector<string> tokenizedVersion;
    string currLine;
    individuals[0] = new Chromosome(section_count);

    int geneLocation = 0;
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*INITIAL*") == 0)
                break;

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
    individuals[0]->updateFitness(incompatibleSections, sectionPref, profPref, timeSlots);
    weakestIndividualID = 0;
    cout << "First Chromosome Initialized..." << endl;
}

void Population::initPopulationFromFirst( ) {
    cout << "Initiating generation of population...." << endl;

    Chromosome * initChromosome = nullptr;
    Chromosome * postMutation = nullptr;
    Chromosome * postRepair = nullptr;

    for (int i = 1; i < population_size; ++i) {
        if (DEBUG_INIT_POPULATION)
            cout << "Generating individual (" << to_string(i) << ")..." << endl;

        individuals[i] = new Chromosome(individuals[0]);
        initChromosome = individuals[0];
        if (DEBUG_INIT_POPULATION)
            cout << "Pre-Mutation: " << endl << individuals[i]->print( );

        individuals[i]->mutate(sectionProf, section_count, creditTimeSlot, timeSlots, &h, mutation_probability);

        postMutation = individuals[i];

        if (DEBUG_INIT_POPULATION)
            cout << "Post-Mutation: " << endl << individuals[i]->print( ) << endl;

        //After mutation, repair.
        individuals[i]->repair(sectionProf, section_count, creditTimeSlot, timeSlots, &h, incompatibleSections, REPAIR_TRIES);
        postRepair = new Chromosome(individuals[i]);

        if (DEBUG_INIT_POPULATION)
            cout << "Post-Repair: " << endl << individuals[i]->print( ) << endl;

        if (DEBUG_INIT_POPULATION_COMPARED) {
            cout << "GeneID       Initial         Mutated      Repaired" << endl;
            for (int x = 0; x < section_count; ++x) {
                cout << x << ":\t" << initChromosome->print(x) << "\t-->\t" << postMutation->print(x) << "\t-->\t" << postRepair->print(x) << endl;
            }
            cout << "Valid: \t\t\t" << validateChromosome(postMutation) << " --> " << validateChromosome(postRepair) << endl;
            cout << "----------------------------------------------------------------" << endl;

        }
        individuals[i]->updateFitness(incompatibleSections, sectionPref, profPref, timeSlots);
        individualValidity[i] = validateChromosome(individuals[i]);
        cout << i << " valid: " << individualValidity[i] << endl;
        cout << "Fitness for " << i << " is " << individuals[i]->getFitness( ) << endl << endl;
        if (individuals[i]->getFitness( ) < individuals[weakestIndividualID]->getFitness( ))
            weakestIndividualID = i;
    }

    if (initChromosome != NULL)
        delete initChromosome;

    if (postMutation)
        delete postMutation;

    if (postRepair)
        delete postRepair;

    cout << "Population generated..." << endl;
}

void Population::readCoursePref(ifstream &inFile, vector<string> courseList) {
    string currLine;
    //1. Create a temp coursePref list that will be used to populate section pref
    //2. populate the coursePref list using a similar approach as to populating course data
    //3. populate sectionPref from the coursePref

    vector<string> tokenizedVersion;
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*COURSEPREF*") == 0)
                break;

            Utility::CleanWhiteSpace(currLine);
            Utility::LowerCase(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            int courseIndex = find(courseList.begin( ), courseList.end( ), tokenizedVersion.at(0)) - courseList.begin( );
            if (courseIndex < (signed) courseList.size( )) {
                string prefs = tokenizedVersion.at(1);
                Utility::RemoveChar(prefs, '(');
                Utility::RemoveChar(prefs, ')');
                tokenizedVersion = Utility::Tokenize(prefs, '-');
                for (int secIdx = 1; secIdx <= courseSection[courseIndex][0]; ++secIdx) {
                    int sectionID = courseSection[courseIndex][secIdx];
                    sectionPref[sectionID] = new int[3];
                    for (int idx = 0; idx < 3; ++idx) {
                        sectionPref[courseSection[courseIndex][secIdx]][idx] = stoi(tokenizedVersion.at(idx));
                    }//end for idx
                } // end for section
            } else {
                continue;
            }//(courseIndex < courseList.size( ))

        }//while(getline(inFile, currLine))
    }//if (inFile.is_open( ))

}

void Population::readProfPref(ifstream &inFile) {
    string currLine;
    vector<string> tokenizedVersion;

    if (inFile.is_open( )) {
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
            string prefs = tokenizedVersion.at(1);
            Utility::RemoveChar(prefs, '(');
            Utility::RemoveChar(prefs, ')');
            tokenizedVersion = Utility::Tokenize(prefs, '-');
            for (int idx = 0; idx < 3; ++idx) {
                profPref[profIndex][idx] = stoi(tokenizedVersion.at(idx));
            }//end for

        }//end while (getline(inFile, currLine))

    }//end if(inFile.is_open( ))
}

bool Population::validateChromosome(Chromosome *toValidate) {
    bool valid = true;
    /*
    In this method, I am just looking for any hard conflicts. First, I will try to ensure that
    a professor is not scheduled twice at the same time. Then, I will look to ensure that no incompatible
    classes are scheduled at the same time.
    */

    for (int left = 0; left < section_count && valid; ++left) {
        int leftProf = toValidate->getProf(left);
        for (int right = left + 1; right < section_count && valid; ++right) {
            int rightProf = toValidate->getProf(right);
            if (leftProf == rightProf) {
                int leftTime = toValidate->getTime(left), rightTime = toValidate->getTime(right);
                if (leftTime == rightTime) {
                    if (DEBUG_VALIDATE)
                        cout << "Incompatible Prof: " << left << " with " << right << endl;
                    valid = false;
                    break;
                }
            }
        }
    }

    for (int left = 0; left < section_count && valid; ++left) {
        int leftTime = toValidate->getTime(left);
        for (int right = 1; right <= incompatibleSections[left][0] && valid; ++right) {
            if (incompatibleSections[left][right] <= left)
                continue;
            int rightTime = toValidate->getTime(incompatibleSections[left][right]);
            if (leftTime == rightTime) {
                if (DEBUG_VALIDATE)
                    cout << "Incompatible Sections: " << left << " with " << right - 1 << endl;
                valid = false;
                break;
            }
        }
    }


    return valid;
}

void Population::Evolve( ) {
    int generationSinceLastReplacement = 0;
    for (int i = 1; i <= generation_count && generationSinceLastReplacement < replacement_wait; ++i) {
        if (generationSinceLastReplacement <= 1) {
            cout << "Current Generation: " << i << " Generations since last replacement: " << generationSinceLastReplacement;
            if (generationSinceLastReplacement == 1) {
                cout << "...";
            } else
                cout << endl;
        }
        int sacrificeID = h.randNum(0, population_size - 1);
        Chromosome * sacrifice = new Chromosome(individuals[sacrificeID]);

        if (DEBUG_EVOLVE) {
            /*for (int i = 0; i < population_size; i++)
                cout << individuals[i]->print( ) << endl << "************************************" << endl;*/
            cout << "Sacrifice source: " << sacrificeID << endl;
            cout << "Sacrifice: " << endl << sacrifice->print( ) << "Source: " << endl << individuals[sacrificeID]->print( ) << endl;
        }

        sacrifice->mutate(sectionProf, section_count, creditTimeSlot, timeSlots, &h, mutation_probability);
        sacrifice->repair(sectionProf, section_count, creditTimeSlot, timeSlots, &h, incompatibleSections, REPAIR_TRIES);
        sacrifice->updateFitness(incompatibleSections, sectionPref, profPref, timeSlots);
        if (sacrifice->getFitness( ) > individuals[weakestIndividualID]->getFitness( )) {
            if (generationSinceLastReplacement > 1 ) {
                cout << endl << "Current Generation: " << i << " Generations since last replacement: " << generationSinceLastReplacement << endl;
            } else if (generationSinceLastReplacement == 1) {
                cout << " Replaced" << endl;
            } else if (generationSinceLastReplacement == 0) {
                cout << endl;
            }
            generationSinceLastReplacement = 0;
            //delete individuals[weakestIndividualID];
            if (DEBUG_EVOLVE) {
                cout << "Replacing " << weakestIndividualID << " with the sacrifice." << endl;
                cout << "Original\t\tNew" << endl;
                for (int i = 0; i < section_count; ++i)
                    cout << individuals[weakestIndividualID]->print(i) << "  ->  " << sacrifice->print(i) << endl;

                cout << "New weakest: " << endl;
            }

            individuals[weakestIndividualID] = new Chromosome(sacrifice);

            if (DEBUG_EVOLVE)
                cout << individuals[weakestIndividualID]->print( );

            weakestIndividualID = 0;
            for (int i = 0; i < population_size; ++i) {
                if (individuals[i]->getFitness( ) < individuals[weakestIndividualID]->getFitness( )) {
                    weakestIndividualID = i;
                }
            }
        } else
            generationSinceLastReplacement++;

        if (DEBUG_EVOLVE) {
            cout << "Current State" << endl;
            for (int i = 0; i < population_size; ++i) {
                cout << "Individual " << i << endl;
                cout << individuals[i]->print( );
            }
            cout << "-----------------------------------------------------------" << endl;
        }
        delete sacrifice;
    }
}

string Population::PrintTableFormat( ) {
    string rtnVal = "";

    if (DEBUG_PRINTTABLE) {
        for (int i = 0; i < 1; ++i) {
            rtnVal += individuals[i]->printTable(timeSlots, timeslot_count);
            rtnVal += "\n\n";
        }
    } else {
        for (int i = 0; i < population_size; ++i) {
            rtnVal += individuals[i]->printTable(timeSlots, timeslot_count);
            rtnVal += "\n\n";
        }
    }

    return rtnVal;
}

ostream & operator<<(ostream & os, const Population &source) {
    os << "Printing Current Population..." << endl;
    for (int i = 0; i < source.population_size; ++i) {
        os << "Individual " << i << endl;
        os << source.individuals[i]->print( ) << endl;
        os << "Fitnesss: " << source.individuals[i]->getFitness( ) << endl;
        os << "***************************************" << endl;
    }

    return os;
}