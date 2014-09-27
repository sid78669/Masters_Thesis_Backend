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
    cout << "First Valid: " << validateChromosome(individuals[0]) << endl;
    //Now we have enough basic information that we can populate our individuals based on the first one.
    initPopulationFromFirst( );

    //Calculate the fitness of each individual
    /*for (int i = 0; i < population_size; i++) {
        individuals[i]->calculateFitness( );
        }*/

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
                cout << courseSection[courseLoc][i] << ": ";
                int sectionID = courseSection[courseLoc][i];
                sectionCredit[sectionID] = cred;

                //Copy all the incompatible sections over.
                incompatibleSections[sectionID] = new int[incompatibles.size( ) + 1];
                incompatibleSections[sectionID][0] = incompatibles.size( );
                for (int j = 1; j <= (signed) incompatibles.size( ); j++) {
                    cout << ", " << incompatibles.at(j - 1);
                    incompatibleSections[sectionID][j] = incompatibles.at(j - 1);
                }
                cout << endl;
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
        for (int i = 0; i < section_count; i++) {
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
            while (!cTimeSlot.empty( ) && creditLoc < (signed) cTimeSlot.size( ) && tempArray[cTimeSlot.at(creditLoc).at(0)]->getCredits( ) != creds)
                creditLoc++;
            if (creditLoc >= (signed) cTimeSlot.size( )) {
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
    cout << "Initiating generation of population...." << endl;

    Chromosome * initChromosome;
    Chromosome * postMutation;
    Chromosome * postRepair;

    for (int i = 1; i < population_size; i++) {
        if (DEBUG_INIT_POPULATION)
            cout << "Generating individual (" << to_string(i) << ")..." << endl;

        individuals[i] = new Chromosome(individuals[0]);
        initChromosome = new Chromosome(individuals[0]);
        if (DEBUG_INIT_POPULATION)
            cout << "Pre-Mutation: " << endl << individuals[i]->print( );

        Chromosome * temp = mutate(i);
        delete individuals[i];
        individuals[i] = new Chromosome(temp);
        postMutation = new Chromosome(temp);
        delete temp;

        if (DEBUG_INIT_POPULATION)
            cout << "Post-Mutation: " << endl << individuals[i]->print( ) << endl;

        //After mutation, repair.
        individuals[i] = repairChromosome(individuals[i]);
        postRepair = new Chromosome(individuals[i]);

        if (DEBUG_INIT_POPULATION)
            cout << "Post-Repair: " << endl << individuals[i]->print( ) << endl;

        if (DEBUG_INIT_POPULATION_COMPARED) {
            cout << "GeneID       Initial         Mutated      Repaired" << endl;
            for (int x = 0; x < section_count; x++) {
                cout << x << ":\t" << initChromosome->print(x) << "\t-->\t" << postMutation->print(x) << "\t-->\t" << postRepair->print(x) << endl;
            }
            cout << "Valid: \t\t\t" << validateChromosome(postMutation) << " --> " << validateChromosome(postRepair) << endl;
            cout << "----------------------------------------------------------------" << endl;

        }
    }

    cout << "Population generated..." << endl;
}

Chromosome * Population::mutate(int individualID) {
    cout << "Mutating Individual: " << individualID << endl;
    Chromosome * mutatedChromosome = new Chromosome(individuals[individualID]);

    for (int g = 0; g < section_count; g++) {
        int creditRow = 0, timeID = -1, compTimes = -1, profID = -1, compProfs = -1;

        compProfs = sectionProf[g][0];
        profID = mutatedChromosome->getProf(g);

        while (timeSlots[creditTimeSlot[creditRow][1]]->getCredits( ) != timeSlots[mutatedChromosome->getTime(g)]->getCredits( ))
            creditRow++;
        compTimes = creditTimeSlot[creditRow][0];
        timeID = mutatedChromosome->getTime(g);

        int mutationTime = h.randNum(1, 100);
        if (mutationTime <= (int) (mutation_probability * 100)) {
            timeID = h.randNum(1, compTimes);
            mutatedChromosome->setTime(g, creditTimeSlot[creditRow][timeID]);
        }

        int mutationProf = h.randNum(1, 100);
        if (mutationProf <= (int) (mutation_probability * 100)) {
            profID = h.randNum(1, compProfs);
            mutatedChromosome->setProf(g, sectionProf[g][profID]);
        }
    }

    cout << "Finished Mutating." << endl;
    return mutatedChromosome;
}

Chromosome * Population::repairChromosome(Chromosome * broken) {
    /*
    Do a loop for maximum of REPAIR_TRIES. Each time keep a boolean
    that will keep track of whether repairs were made or not. If
    after an iteration no repairs have been made, then the individual
    is good and repair is complete.

    To perform repair, follow the taboo list principle. Do a look
    ahead to see if there is anything that will hard conflict with the
    current gene. If it will, replace the gene with other options, and
    put the current values into the taboo list. Only look for genes on
    the left that will conflict, don't worry about what we have already
    seen.

    If an individual cannot be repaired, ignore it. It will probably
    not replace anything as the fitness will be much lower for it. If
    it replaces anything, than it was probably for the best.
    */

    bool currentIterationRepaired = false;

    int tries = 0;

    vector<int> * tabooProf = new vector<int>[section_count];
    vector<int> * tabooTime = new vector<int>[section_count];
    //We want to go through the Chromosome, at least once.
    do {
        for (int secIndex = 0; secIndex < section_count; secIndex++) {
            Gene left = broken->getGene(secIndex);
            Gene right;
            for (int conflictSection = 1; conflictSection <= incompatibleSections[secIndex][0]; conflictSection++) {
                int conflictID = incompatibleSections[secIndex][conflictSection];

                //We want to ensure that this is a look ahead search. Hence, if the conflicting section is
                //On the left of the current section, skip this iteration.
                if (conflictID < secIndex)
                    continue;
                right = broken->getGene(conflictID);

                //Check if time and prof conflict. In that case, switch prof
                if (left.getTimeID( ) == right.getTimeID( ) && left.getProfID( ) == right.getProfID( )) {
                    cout << "Time and Prof Collision for " << left.getTimeID( ) << " and " << right.getTimeID( ) << endl;
                    currentIterationRepaired = true;

                    //Add the professor to the prof taboo list for the conflicting section
                    tabooProf[conflictID].push_back(right.getTimeID( ));
                    
                    int compProfs = sectionProf[conflictID][0]; 

                    //We want to only loop as many times as the possible professors that we have.
                    int looped = compProfs;
                    do {
                        //Get a random professor from the list of valid profs.
                        int profID = sectionProf[conflictID][h.randNum(1, compProfs)];

                        //Check if the new prof selected is in the taboo list.
                        if (find(tabooProf->begin( ), tabooProf->end( ), profID) == tabooProf->end( )) {
                            //If the prof is not in the taboo list, set it as the new prof and break out of the loop.
                            right.setProfID(profID);
                            break;
                        }
                    } while (--looped >= 0);

                } else if (left.getTimeID( ) == right.getTimeID( )) {
                    //Add the current time to the taboo list for time for the conflicting section
                    tabooTime[conflictID].push_back(right.getTimeID( ));

                    int timeRow = 0;
                    //Look for the time row with the correct credit rating.
                    while (timeSlots[creditTimeSlot[timeRow][0]]->getCredits( ) != timeSlots[right.getTimeID( )]->getCredits( ))
                        timeRow++;

                    int compTimes = creditTimeSlot[timeRow][0];

                    //We only want to loop as many times as the possible timeslots that can accomodate this section
                    int looped = compTimes;
                    do {
                        //Get a random time from the list timeslots for the given number of credits.
                        int timeID = creditTimeSlot[timeRow][h.randNum(1, compTimes)];

                        //Check if the time is in the taboo list or not.
                        if (find(tabooTime->begin( ), tabooTime->end( ), timeID) == tabooTime->end( )) {
                            right.setTimeID(timeID);
                            break;
                        }

                    } while (--looped >= 0);
                }
                broken->setGene(conflictID, right);
            }
        }
        cout << tries << "Any Repairs? " << currentIterationRepaired << endl;
    } while (++tries < REPAIR_TRIES && currentIterationRepaired);
    //cout << "Repaird: " << broken->print( ) << endl;
    cout << "Gene Repaired to the best of the abilities..." << endl;
    return broken;
}

bool Population::validateChromosome(Chromosome *toValidate) {
    bool valid = true;
    /*
    In this method, I am just looking for any hard conflicts. First, I will try to ensure that
    a professor is not scheduled twice at the same time. Then, I will look to ensure that no incompatible
    classes are scheduled at the same time.
    */

    for (int left = 0; left < section_count && valid; left++) {
        int leftProf = toValidate->getProf(left);
        for (int right = left + 1; right < section_count && valid; right++) {
            int rightProf = toValidate->getProf(right);
            if (leftProf == rightProf) {
                int leftTime = toValidate->getTime(left), rightTime = toValidate->getTime(right);
                if (leftTime == rightTime) {
                    cout << "Incompatible Prof: " << left << " with " << right << endl;
                    valid = false;
                    break;
                }
            }
        }
    }

    for (int left = 0; left < section_count && valid; left++) {
        int leftTime = toValidate->getTime(left);
        for (int right = 1; right <= incompatibleSections[left][0] && valid; right++) {
            int rightTime = toValidate->getTime(incompatibleSections[left][right]);
            if (leftTime == rightTime) {
                cout << "Incompatible Sections: " << left << " with " << right - 1 << endl;
                valid = false;
                break;
            }
        }
    }


    return valid;
}