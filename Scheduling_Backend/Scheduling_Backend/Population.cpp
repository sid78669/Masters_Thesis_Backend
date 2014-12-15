#include "Population.h"

//Population::Population(string dataFilePath, int populationSize, int generationCount, int replacementWait, double mutationProbability) {
Population::Population(string dataFilePath) :
REPAIR_TRIES(5) {
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
    //Setup the Individuals array
    readDatFiles( );

} //end Constructor

Population::~Population( ) {
    if (individuals) {
        for (int i = 0; i < population_size; ++i) {
            delete individuals[i];
        }
        delete[] individuals;
    }
    if (timeSlots) {
        for (int i = 0; i < timeslot_count; ++i) {
            delete timeSlots[i];
        }
        delete[] timeSlots;
    }
    for (int i = 0; i < section_count; ++i) {
        delete[] incompatibleSections[i];
    } //end for
    delete[] incompatibleSections;

    for (int i = 0; i < credit_count; ++i) {
        delete[] creditTimeSlot[i];
    }

    delete[] creditTimeSlot;

    for (int i = 0; i < section_count; ++i) {
        delete[] sectionProf[i];
    }

    delete[] sectionProf;

    for (int i = 0; i < course_count; ++i) {
        delete[] courseSection[i];
    }

    delete[] courseSection;

    delete[] sectionCredit;

    for (int i = 0; i < section_count; ++i) {
        delete[] sectionPref[i];
    }

    delete[] sectionPref;

    for (int i = 0; i < professor_count; ++i) {
        delete[] profPref[i];
    }

    delete[] profPref;

    delete[] individualValidity;

    delete[] profSectionsTaught;

    delete[] profCreditMax;
} //end destructor

void Population::readDatFiles( ) {
    vector<string> courses;

    ifstream inFile(data_file_path);

    cout << "Beginning Initial Setup...." << endl << endl;

    //Read the parameters
    readParameters(inFile);
    individuals = new Chromosome*[population_size];
    individualValidity = new bool[population_size];

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

    /**** Setup soft constraints ****/
    //Read the course time preferences
    readCoursePref(inFile, courses);

    //Read the prof time preferences
    readProfPref(inFile);

    //FINALLY! Use the given chromosome blueprint to create the first chromosome
    readInitialSchedule(inFile);
    cout << endl;
    weakestIndividualID = 0;

    individualValidity[0] = validateChromosome(individuals[0]);
    cout << "First Valid: " << individualValidity[0] << endl << endl;

    //Now we have enough basic information that we can populate our individuals based on the first one.
    initPopulationFromFirst( );

    //Output all individuals
    if (DEBUG_INIT)
        for (int i = 0; i < population_size; ++i) {
            cout << "Individual " << i << endl;
            cout << individuals[i]->print( );
        } //end for

    inFile.close( );

    cout << endl << "Initial Setup Complete" << endl << endl;
    cout << "Generation, ";
    for (int x = 0; x < population_size; ++x) {
        cout << to_string(x) << ",";
    }

    cout << "Mean, SD;" << endl;

    cout << "0," << GetFitnessData( ) << endl;
} //end readDatFiles()

void Population::readParameters(ifstream &inFile) {
    string currLine;
    if (inFile.is_open( )) {
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
                cout << currLine << "> " << tokenizedVersion[0] << "#"
                << tokenizedVersion[1] << "** " << endl;
        } //end while
    } //end if

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
        } //end for

        //Delete the temp sub array for that index.
        if (DEBUG_SECTION)
            cout << endl;
    } //end for

    //delete the temp helper array
    delete[] tempCourseSections;
    course_count = courseList.size( );
    cout << "New Courses Added: " << course_count << endl
        << "New Sections Added: " << section_count << endl;
} //end readSectionList

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
            int courseLoc = find(courseList.begin( ), courseList.end( ),
                                 tokenizedVersion.at(0)) - courseList.begin( );

            //Setup data that will be same between related sections, i.e. credits and incompatible sections.
            double cred = stod(tokenizedVersion.at(1));
            vector<int> incompatibles;
            //Check if there are any incompatible sections.
            if (tokenizedVersion.size( ) > 2) {
                for (int inc = 2; inc < (signed) tokenizedVersion.size( );
                     ++inc) {
                    //Find the courseID
                    /*int currIncomp = 0;
                    while (courseList.at(currIncomp).compare(tokenizedVersion.at(inc)) != 0)
                    currIncomp++;*/
                    int currIncomp = find(courseList.begin( ), courseList.end( ),
                                          tokenizedVersion.at(0)) - courseList.begin( );

                    //Add the half sections for this course to the vector
                    for (int secIdx = 1;
                         secIdx <= courseSection[currIncomp][0] / 2;
                         ++secIdx) {
                        incompatibles.push_back(
                            courseSection[currIncomp][secIdx]);
                    } //end for
                } //end for
            } //end if

            if (DEBUG_COURSES) {
                for (auto inco : incompatibles)
                    cout << inco << ", ";
                cout << endl;
            } //end if

            //Loop through the sections and set the values for credits and incompatible sections
            for (int i = 1; i <= courseSection[courseLoc][0]; ++i) {
                if (DEBUG_COURSES)
                    cout << courseSection[courseLoc][i] << ": ";
                int sectionID = courseSection[courseLoc][i];
                sectionCredit[sectionID] = cred;

                //Copy all the incompatible sections over.
                incompatibleSections[sectionID] = new int[incompatibles.size( )
                    + 1];
                incompatibleSections[sectionID][0] = incompatibles.size( );
                for (int j = 1; j <= (signed) incompatibles.size( ); ++j) {
                    if (DEBUG_COURSES)
                        cout << ", " << incompatibles.at(j - 1);
                    incompatibleSections[sectionID][j] = incompatibles.at(
                        j - 1);
                } //end for
                if (DEBUG_COURSES)
                    cout << endl;
            } //end for
        } //end while
    } //end if

    cout << "Finished reading courses. Updated " << course_count
        << " new courses." << endl;
} //end readCourseList

void Population::readProfessorList(ifstream &inFile,
                                   vector<string> courseList) {
    cout << "Starting reading professors...." << endl;

    string currLine;
    vector<int> * secProfs = new vector<int>[section_count];
    vector<double> maxCredits;
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
            tokenizedVersion.erase(tokenizedVersion.begin( ));
            maxCredits.push_back(stod(tokenizedVersion[0]));
            tokenizedVersion.erase(tokenizedVersion.begin( ));
            for (int courseLoc = 0;
                 courseLoc < (signed) tokenizedVersion.size( ); ++courseLoc) {
                int courseID = find(courseList.begin( ), courseList.end( ),
                                    tokenizedVersion.at(courseLoc)) - courseList.begin( );
                if (DEBUG_PROF)
                    cout << "Course: " << tokenizedVersion.at(courseLoc)
                    << " ID: " << courseID << endl;
                for (int sects = 1; sects <= courseSection[courseID][0];
                     ++sects) {
                    secProfs[courseSection[courseID][sects]].push_back(
                        professor_count);
                    if (DEBUG_PROF)
                        cout << "Section: " << courseSection[courseID][sects]
                        << " Prof: " << professor_count << endl;
                }
            }
            professor_count++;
        }
        profSectionsTaught = new int[professor_count]( );
        profCreditMax = new double[professor_count]( );
        for (int z = 0; z < professor_count; z++) {
            profCreditMax[z] = maxCredits[z];
        }
        for (int i = 0; i < section_count; ++i) {
            if (DEBUG_PROF)
                cout << i << "---" << endl;
            sectionProf[i] = new int[secProfs[i].size( ) + 1];
            sectionProf[i][0] = secProfs[i].size( );
            int cntr = 1;
            for (auto sp : secProfs[i]) {
                sectionProf[i][cntr++] = sp;
                profSectionsTaught[sp]++;
                if (DEBUG_PROF)
                    cout << sp << ", ";
            }
            if (DEBUG_PROF)
                cout << endl;
        }
        if (DEBUG_PROF_NEW) {
            cout << "Prof Sections taught: " << endl;
            for (int i = 0; i < professor_count; i++) {
                cout << "(" << i << "," << profSectionsTaught[i] << ")" << endl;
            }
            //exit(0);
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
    int timeslotID = 0;
    TimeSlot ** tempArray = new TimeSlot*[ARRAY_MAX];
    if (inFile.is_open( )) {
        while (getline(inFile, currLine)) {
            if (currLine[0] == '/')
                continue;
            if (currLine.compare("*END*TIMESLOT*") == 0)
                break;
            Utility::CleanWhiteSpace(currLine);
            tokenizedVersion = Utility::Tokenize(currLine, ',');
            timeslotID = stoi(tokenizedVersion.at(0));
            while (timeslot_count < timeslotID) {
                tempArray[timeslot_count] = new TimeSlot(-1);
                int creditLoc = 0;
                while (!cTimeSlot.empty( )
                       && creditLoc < (signed) cTimeSlot.size( )
                       && tempArray[cTimeSlot.at(creditLoc).at(0)]->getCredits( )
                       != -1)
                       creditLoc++;
                if (creditLoc >= (signed) cTimeSlot.size( )) {
                    vector<int> newVec;
                    cTimeSlot.push_back(newVec);
                }
                cTimeSlot.at(creditLoc).push_back(timeslot_count);
                timeslot_count++;
            }
            double creds = stod(tokenizedVersion.at(1));
            tempArray[timeslot_count] = new TimeSlot(creds);
            for (int i = 0; i < 6; ++i)
                tempArray[timeslot_count]->setPeriod(i,
                tokenizedVersion.at(2 + i));
            int creditLoc = 0;
            while (!cTimeSlot.empty( ) && creditLoc < (signed) cTimeSlot.size( )
                   && tempArray[cTimeSlot.at(creditLoc).at(0)]->getCredits( )
                   != creds)
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
        timeCredLegend = new double[credit_count];
        for (int i = 0; i < credit_count; ++i) {
            timeCredLegend[i] = timeSlots[cTimeSlot.at(i).at(0)]->getCredits( );
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
    individuals[0] = new Chromosome(section_count, professor_count,
                                    profCreditMax);

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

            Gene current(stoi(tokenizedVersion.at(1)),
                         stoi(tokenizedVersion.at(2)));
            individuals[0]->setGene(geneLocation, current);
            individuals[0]->setProf(geneLocation, stoi(tokenizedVersion.at(1)),
                                    sectionCredit[geneLocation]);
            geneLocation++;
        }
        if (DEBUG_INIT_CHROMOSOME)
            cout << individuals[0]->print( ) << endl;
    }
    individuals[0]->updateFitness(incompatibleSections, sectionPref, profPref,
                                  timeSlots, professor_count, timeslot_count, profSectionsTaught);
    weakestIndividualID = 0;
    lowestFitnessSeen = individuals[0]->getFitness( );
    strongestIndividualID = 0;
    highestFitnessSeen = individuals[0]->getFitness( );

    cout << "Prof Rating: " << endl << individuals[0]->getProfessorLoads( )
        << endl;

    cout << "First Chromosome Initialized..." << endl;
}

void Population::initPopulationFromFirst( ) {
    cout << "Initiating generation of population...." << endl;

    //Chromosome * initChromosome;
    //Chromosome * postMutation;
    //Chromosome * postRepair;

    for (int i = 1; i < population_size; ++i) {
        if (DEBUG_INIT_POPULATION)
            cout << "Generating individual (" << to_string(i) << ")..." << endl;

        individuals[i] = new Chromosome(individuals[0]);
        //initChromosome = individuals[0];
        if (DEBUG_INIT_POPULATION)
            cout << "Pre-Mutation: " << endl << individuals[i]->print( );

        individuals[i]->mutate(sectionProf, section_count, creditTimeSlot,
                               timeSlots, timeCredLegend, credit_count, &h,
                               mutation_probability, sectionCredit);

        //postMutation = individuals[i];

        if (DEBUG_INIT_POPULATION)
            cout << "Post-Mutation: " << endl << individuals[i]->print( )
            << endl;

        //After mutation, repair.
        individuals[i]->repair(sectionProf, section_count, creditTimeSlot,
                               timeSlots, timeCredLegend, credit_count, &h,
                               incompatibleSections, REPAIR_TRIES, sectionCredit);
        //postRepair = new Chromosome(individuals[i]);

        if (DEBUG_INIT_POPULATION)
            cout << "Post-Repair: " << endl << individuals[i]->print( ) << endl;

        /*if (DEBUG_INIT_POPULATION_COMPARED) {
        cout << "GeneID       Initial         Mutated      Repaired"
        << endl;
        for (int x = 0; x < section_count; ++x) {
        cout << x << ":\t" << initChromosome->print(x) << "\t-->\t"
        << postMutation->print(x) << "\t-->\t"
        << postRepair->print(x) << endl;
        }
        cout << "Valid: \t\t\t" << validateChromosome(postMutation)
        << " --> " << validateChromosome(postRepair) << endl;
        cout
        << "----------------------------------------------------------------"
        << endl;

        }*/
        individuals[i]->updateFitness(incompatibleSections, sectionPref,
                                      profPref, timeSlots, professor_count, timeslot_count,
                                      profSectionsTaught);
        individualValidity[i] = validateChromosome(individuals[i]);
        if (DEBUG_INIT_POPULATION) {

            cout << i << " valid: " << individualValidity[i] << endl;
            cout << "Fitness for " << i << " is "
                << individuals[i]->getFitness( ) << endl << endl;
        }
        if (individuals[i]->getFitness( )
            < individuals[weakestIndividualID]->getFitness( )) {
            weakestIndividualID = i;
            lowestFitnessSeen = individuals[i]->getFitness( );
        }

        if (individuals[i]->getFitness( )
            > individuals[strongestIndividualID]->getFitness( )) {
            strongestIndividualID = i;
            highestFitnessSeen = individuals[i]->getFitness( );
        }
    }

    /*if (initChromosome)
    delete initChromosome;

    if (postMutation)
    delete postMutation;

    if (postRepair)
    delete postRepair;*/

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
            int courseIndex = find(courseList.begin( ), courseList.end( ),
                                   tokenizedVersion.at(0)) - courseList.begin( );
            if (courseIndex < (signed) courseList.size( )) {
                string prefs = tokenizedVersion.at(1);
                Utility::RemoveChar(prefs, '(');
                Utility::RemoveChar(prefs, ')');
                tokenizedVersion = Utility::Tokenize(prefs, '-');
                for (int secIdx = 1; secIdx <= courseSection[courseIndex][0];
                     ++secIdx) {
                    int sectionID = courseSection[courseIndex][secIdx];
                    sectionPref[sectionID] = new int[3];
                    for (int idx = 0; idx < 3; ++idx) {
                        sectionPref[courseSection[courseIndex][secIdx]][idx] =
                            stoi(tokenizedVersion.at(idx));
                    } //end for idx
                } // end for section
            } else {
                continue;
            } //(courseIndex < courseList.size( ))

        } //while(getline(inFile, currLine))
    } //if (inFile.is_open( ))

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
            } //end for

        } //end while (getline(inFile, currLine))

    } //end if(inFile.is_open( ))
}

bool Population::validateChromosome(Chromosome * const toValidate) const {
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
                int leftTime = toValidate->getTime(left), rightTime =
                    toValidate->getTime(right);
                if (leftTime == rightTime) {
                    if (DEBUG_VALIDATE)
                        cout << "Incompatible Prof: " << left << " with "
                        << right << endl;
                    valid = false;
                    break;
                }
            }
        }
    }

    for (int pr = 0; pr < professor_count; pr++) {
        if (toValidate->getCourseLoad(pr) < 0)
            valid = false;
    }

    for (int left = 0; left < section_count && valid; ++left) {
        int leftTime = toValidate->getTime(left);
        for (int right = 1; right <= incompatibleSections[left][0] && valid;
             ++right) {
            if (incompatibleSections[left][right] <= left)
                continue;
            int rightTime = toValidate->getTime(
                incompatibleSections[left][right]);
            if (leftTime == rightTime) {
                if (DEBUG_VALIDATE)
                    cout << "Incompatible Sections: " << left << " with "
                    << right - 1 << endl;
                valid = false;
                break;
            }
        }
    }

    return valid;
}

void Population::Evolve( ) {
    int generationSinceLastReplacement = 0;

    int i = 1;
    for (i = 0;
         i <= generation_count
         && generationSinceLastReplacement < replacement_wait; ++i) {
        if (DEBUG_EVOLVE) {
            if (generationSinceLastReplacement <= 1) {
                cout << "Current Generation: " << i
                    << " Generations since last replacement: "
                    << generationSinceLastReplacement;
                if (generationSinceLastReplacement == 1) {
                    cout << "...";
                } else
                    cout << endl;
            }
        }

        int sacrificeID = h.randNum(0, population_size - 1);
        if (individuals[sacrificeID]->getFitness( ) < lowestFitnessSeen)
            lowestFitnessSeen = individuals[sacrificeID]->getFitness( );
        if (individuals[sacrificeID]->getFitness( ) > highestFitnessSeen)
            highestFitnessSeen = individuals[sacrificeID]->getFitness( );
        Chromosome * sacrifice = new Chromosome(individuals[sacrificeID]);

        if (DEBUG_EVOLVE) {
            /*for (int i = 0; i < population_size; i++)
            cout << individuals[i]->print( ) << endl << "************************************" << endl;*/
            cout << "Sacrifice source: " << sacrificeID << endl;
            cout << "Sacrifice: " << endl << sacrifice->print( ) << "Source: "
                << endl << individuals[sacrificeID]->print( ) << endl;
        }

        sacrifice->mutate(sectionProf, section_count, creditTimeSlot, timeSlots,
                          timeCredLegend, credit_count, &h, mutation_probability,
                          sectionCredit);
        sacrifice->repair(sectionProf, section_count, creditTimeSlot, timeSlots,
                          timeCredLegend, credit_count, &h, incompatibleSections,
                          REPAIR_TRIES, sectionCredit);
        sacrifice->updateFitness(incompatibleSections, sectionPref, profPref,
                                 timeSlots, professor_count, timeslot_count, profSectionsTaught);

        if (sacrifice->getFitness( )
         > individuals[weakestIndividualID]->getFitness( )) {
            if (DEBUG_EVOLVE) {
                if (DEBUG_EVOLVE && generationSinceLastReplacement > 1) {
                    cout << endl << "Current Generation: " << i
                        << " Generations since last replacement: "
                        << generationSinceLastReplacement << endl;
                } else if (DEBUG_EVOLVE
                           && generationSinceLastReplacement == 1) {
                    cout << " Replaced" << endl;
                } else if (DEBUG_EVOLVE
                           && generationSinceLastReplacement == 0) {
                    cout << endl;
                }
            }
            generationSinceLastReplacement = 0;

            if (DEBUG_EVOLVE) {
                cout << "Replacing " << weakestIndividualID
                    << " with the sacrifice." << endl;
                cout << "Original\t\tNew" << endl;
                for (int i = 0; i < section_count; ++i)
                    cout << individuals[weakestIndividualID]->print(i)
                    << "  ->  " << sacrifice->print(i) << endl;

                cout << "New weakest: " << endl;
            }

            individuals[weakestIndividualID] = new Chromosome(sacrifice);

            if (DEBUG_EVOLVE)
                cout << individuals[weakestIndividualID]->print( );

            for (int j = 0; j < population_size; ++j) {
                if (individuals[j]->getFitness( )
                    < individuals[weakestIndividualID]->getFitness( )) {
                    weakestIndividualID = j;
                }
                if (individuals[j]->getFitness( )
                    > individuals[strongestIndividualID]->getFitness( )) {
                    strongestIndividualID = j;
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
            cout
                << "-----------------------------------------------------------"
                << endl;
        }
        delete sacrifice;

        if (i > 0 && ((i % 100 == 0) || i == 1)) {
            cout << i << ",";
            cout << GetFitnessData( );
        }
    }
    cout << i << ",";
    cout << GetFitnessData( );
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

string Population::GetFitnessData( ) {
    int * allFitness = new int[population_size - 2];
    stringstream s;
    for (int i = 0; i < population_size; ++i) {
        allFitness[i - 1] = individuals[i]->getFitness( );
        s << individuals[i]->getFitness( ) << ",";
    }
    double mean = Utility::CalculateMean(allFitness, population_size-1);
    double sd = Utility::StandardDeviation(allFitness, population_size-1, mean);
    s << mean << "," << sd;
    s << ";\n";
    return s.str( );
}

ostream & operator<<(ostream & os, const Population &source) {
    if (PRINT_WHOLE_POPULATION) {
        os << "Printing Current Population..." << endl;
        int* fitnessValues = new int[source.population_size];
        for (int i = 0; i < source.population_size; ++i) {
            os << "Individual " << i << endl;
            os
                << source.individuals[i]->printTable(source.timeSlots,
                source.timeslot_count);
            fitnessValues[i] = source.individuals[i]->getFitness( );
            os << "Fitnesss: " << fitnessValues[i] << endl;
            os << "Valid: "
                << (source.validateChromosome(source.individuals[i]) ?
                "Yes" : "No") << endl;
            os << "*******************************************************"
                << endl << endl;
        }
        os << "Current fitness values: " << endl;
        for (int i = 0; i < source.population_size - 1; ++i) {
            os << fitnessValues[i] << ",";
        }
        os << fitnessValues[source.population_size - 1] << endl;
    }
    os << "\n\nStrongest Individual: " << source.strongestIndividualID << endl;
    os
        << source.individuals[source.strongestIndividualID]->printTable(
        source.timeSlots, source.timeslot_count);
    os << "Fitnesss: "
        << source.individuals[source.strongestIndividualID]->getFitness( )
        << endl;
    bool strongestValidity = source.validateChromosome(
        source.individuals[source.strongestIndividualID]);
    os << "Valid: " << (strongestValidity ? "Yes" : "No") << endl;
    os << "Professor Credits: "
        << source.individuals[source.strongestIndividualID]->getProfessorLoads( )
        << endl;

    if (!strongestValidity) {

        int maxFitness = 0, maxFitID = 0;
        bool validFound = false;
        for (int i = 0; i < source.population_size; ++i) {
            if (source.validateChromosome(source.individuals[i])
                && source.individuals[i]->getFitness( ) > maxFitness) {
                validFound = true;
                maxFitID = i;
                maxFitness = source.individuals[i]->getFitness( );
            }
        }
        if (maxFitness == 0) {
            maxFitness = source.individuals[maxFitID]->getFitness( );
        }
        if (maxFitness < source.individuals[source.strongestIndividualID]->getFitness( )) {
            os << "\n\n Strongest individual is the that was found.\n" << endl;
        } else {
            os << "\n\nStrongest Valid Individual: " << maxFitID << endl;
            os
                << source.individuals[maxFitID]->printTable(source.timeSlots,
                source.timeslot_count);
            os << "Fitnesss: " << maxFitness << endl;
            os << "Valid: " << (validFound ? "Yes" : "No") << endl;
            os << "Professor Credits: "
                << source.individuals[maxFitID]->getProfessorLoads( ) << endl;
        }
    } else {
        os << "Strongest was valid." << endl;

    }

    os << "Done." << endl;
    return os;
}
