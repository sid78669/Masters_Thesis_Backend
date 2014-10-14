#include "Chromosome.h"


Chromosome::Chromosome(int geneLength) {
    gene_length = geneLength;
    fitness = 0;
    genes = new Gene[geneLength];
}

Chromosome::Chromosome(const Chromosome * other) {
    gene_length = other->gene_length;
    genes = new Gene[gene_length];
    fitness = other->fitness;
    for (int i = 0; i < gene_length; i++) {
        setGene(i, other->genes[i]);
    }
}

Chromosome::Chromosome(const Chromosome &source) {
    cout << "Overloaded Copy Constructor" << endl;
    gene_length = source.gene_length;
    genes = new Gene[gene_length];
    fitness = source.fitness;
    for (int i = 0; i < gene_length; i++) {
        setGene(i, source.genes[i]);
    }
}

Chromosome& Chromosome::operator=(const Chromosome &source) {
    gene_length = source.gene_length;
    genes = new Gene[gene_length];
    fitness = source.fitness;
    for (int i = 0; i < gene_length; i++) {
        setGene(i, source.genes[i]);
    }

    if (DEBUG_ASSIGNMENT) {
        cout << "source\t\tcurrent" << endl;
        for (int i = 0; i < gene_length; i++) {
            cout << source.genes[i] << "\t\t\t" << this->genes[i] << endl;
        }
    }
    return *this;
}


Chromosome::~Chromosome( ) {
    if (genes != NULL)
    delete[] genes;
}

void Chromosome::setTime(int geneID, int newTime) {
    genes[geneID].setTimeID(newTime);
}

void Chromosome::setProf(int geneID, int newProf) {
    genes[geneID].setProfID(newProf);
}

void Chromosome::setGene(int geneID, Gene newGene) {
    genes[geneID].setProfID(newGene.getProfID( ));
    genes[geneID].setTimeID(newGene.getTimeID( ));
}

int Chromosome::getTime(int geneID) {
    return genes[geneID].getTimeID( );
}

int Chromosome::getProf(int geneID) {
    return genes[geneID].getProfID( );
}

Gene* Chromosome::getGene(int geneID) {
    return &genes[geneID];
}

int Chromosome::getFitness( ) {
    return fitness;
}

string Chromosome::print( ) {
    string rtnVal;
    for (int i = 0; i < gene_length; i++) {
        rtnVal += to_string(i) + "> " + to_string(genes[i].getProfID( )) + ", " + to_string(genes[i].getTimeID( )) + "\n";
    }

    rtnVal += "----------------------------\n";

    return rtnVal;
}

string Chromosome::print(int geneID) {
    return to_string(genes[geneID].getProfID( )) + ", " + to_string(genes[geneID].getTimeID( ));
}

void Chromosome::mutate(int ** sectionProf, int section_count, int ** creditTimeSlot, TimeSlot ** timeSlots, Helper * h, double mutation_probability) {
    if (DEBUG_MUTATION)
        cout << "Mutating Individual... " << endl;
    
    for (int g = 0; g < section_count; g++) {
        int creditRow = 0, timeID = -1, compTimes = -1, profID = -1, compProfs = -1;

        compProfs = sectionProf[g][0];
        profID = getProf(g);

        while (timeSlots[creditTimeSlot[creditRow][1]]->getCredits( ) != timeSlots[getTime(g)]->getCredits( ))
            creditRow++;
        compTimes = creditTimeSlot[creditRow][0];
        timeID = getTime(g);

        int mutationTime = h->randNum(1, 100);
        if (mutationTime <= (int) (mutation_probability * 100)) {
            if (compTimes != 1)
                timeID = h->randNum(1, compTimes);
            else
                timeID = 1;
            setTime(g, creditTimeSlot[creditRow][timeID]);
        }

        int mutationProf = h->randNum(1, 100);
        if (mutationProf <= (int) (mutation_probability * 100)) {
            if (compProfs != 1)
                profID = h->randNum(1, compProfs);
            else
                profID = 1;
            setProf(g, sectionProf[g][profID]);
        }
    }

    if (DEBUG_MUTATION)
        cout << "Finished Mutating." << endl;
}

void Chromosome::repair(int ** sectionProf, int section_count, int ** creditTimeSlot, TimeSlot ** timeSlots, Helper * h, int ** incompatibleSections, const int REPAIR_TRIES) {
    /*
    //    Do a loop for maximum of REPAIR_TRIES. Each time keep a boolean
    //    that will keep track of whether repairs were made or not. If
    //    after an iteration no repairs have been made, then the individual
    //    is good and repair is complete.
    //
    //    To perform repair, follow the taboo list principle. Do a look
    //    ahead to see if there is anything that will hard conflict with the
    //    current gene. If it will, replace the gene with other options, and
    //    put the current values into the taboo list. Only look for genes on
    //    the left that will conflict, don't worry about what we have already
    //    seen.
    //
    //    If an individual cannot be repaired, ignore it. It will probably
    //    not replace anything as the fitness will be much lower for it. If
    //    it replaces anything, than it was probably for the best.
    //    */

    bool currentIterationRepaired = false;

    int tries = 0;

    vector<int> * tabooProf = new vector<int>[section_count];
    vector<int> * tabooTime = new vector<int>[section_count];
    //We want to go through the Chromosome, at least once.
    do {
        for (int secIndex = 0; secIndex < section_count; secIndex++) {
            int right = -1;
            
            for (int conflictSection = 1; conflictSection <= incompatibleSections[secIndex][0]; conflictSection++) {
                int right = incompatibleSections[secIndex][conflictSection];

                //We want to ensure that this is a look ahead search. Hence, if the conflicting section is
                //On the left of the current section, skip this iteration.
                if (right <= secIndex)
                    continue;
            
                //Check if time and prof conflict. In that case, switch prof
                if (genes[secIndex].getTimeID( ) == genes[right].getTimeID( ) && genes[secIndex].getProfID( ) == genes[right].getProfID( )) {
                    if (DEBUG_REPAIR)
                        cout << "Time and Prof Collision for " << genes[secIndex].getTimeID( ) << " and " << genes[right].getTimeID( ) << endl;
                    currentIterationRepaired = true;

                    //Add the professor to the prof taboo list for the conflicting section
                    tabooProf[right].push_back(genes[right].getTimeID( ));

                    int compProfs = sectionProf[right][0];

                    //We want to only loop as many times as the possible professors that we have.
                    int looped = compProfs;
                    do {
                        //Get a random professor from the list of valid profs.
                        int profID = -1;
                        if (compProfs == 1) {
                            profID = sectionProf[right][1];
                        } else {
                            profID = sectionProf[right][h->randNum(1, compProfs)];
                        }

                        //Check if the new prof selected is in the taboo list.
                        if (find(tabooProf->begin( ), tabooProf->end( ), profID) == tabooProf->end( )) {
                            //If the prof is not in the taboo list, set it as the new prof and break out of the loop.
                            genes[right].setProfID(profID);
                            break;
                        }
                    } while (--looped >= 0);

                } else if (genes[secIndex].getTimeID( ) == genes[right].getTimeID( )) {
                    //Add the current time to the taboo list for time for the conflicting section
                    tabooTime[right].push_back(genes[right].getTimeID( ));

                    int timeRow = 0;
                    //Look for the time row with the correct credit rating.
                    while (timeSlots[creditTimeSlot[timeRow][0]]->getCredits( ) != timeSlots[genes[right].getTimeID( )]->getCredits( ))
                        timeRow++;

                    int compTimes = creditTimeSlot[timeRow][0];

                    //We only want to loop as many times as the possible timeslots that can accomodate this section
                    int looped = compTimes;
                    do {
                        //Get a random time from the list timeslots for the given number of credits.
                        int timeID = -1;
                        if (compTimes != 1) {
                            timeID = creditTimeSlot[timeRow][h->randNum(1, compTimes)];
                        } else
                            timeID = creditTimeSlot[timeRow][1];

                        //Check if the time is in the taboo list or not.
                        if (find(tabooTime->begin( ), tabooTime->end( ), timeID) == tabooTime->end( )) {
                            genes[right].setTimeID(timeID);
                            break;
                        }

                    } while (--looped >= 0);
                }
                setGene(right, genes[right]);
            }
        }
        if (tries > 0 && DEBUG_REPAIR)
            cout << "Try " << tries << ": Any Repairs? " << currentIterationRepaired << endl;
    } while (++tries < REPAIR_TRIES && currentIterationRepaired);
    
    if (DEBUG_REPAIR)
        cout << "Gene Repaired to the best of the abilities..." << endl;
}

void Chromosome::updateFitness(int ** incompatibleSections, int ** sectionPref, int ** profPref, TimeSlot ** timeSlots) {
    fitness = 10000;
    
    //Check for hard constraint violation for professor
    for (int left = 0; left < gene_length; left++) {
        if(left < 0)
            cout << "Inside loop: " << gene_length;
        int leftProf = genes[left].getProfID( );
        for (int right = left + 1; right < gene_length; right++) {
            int rightProf = genes[right].getProfID( );
            if (leftProf == rightProf) {
                int leftTime = genes[left].getTimeID( ), rightTime = genes[right].getTimeID( );
                if (leftTime == rightTime) {
                    if (DEBUG_FITNESS)
                        cout << "Incompatible Prof Time: " << left << " with " << right << endl;
                    fitness -= 200;
                }
            }
        }
    }

    //Check for hard constraint violation for time conflict
    for (int left = 0; left < gene_length; left++) {
        if (left < 0)
            cout << "Inside loop2: " << gene_length;
        int leftTime = genes[left].getTimeID( );
        for (int right = 1; right <= incompatibleSections[left][0]; right++) {
            if (incompatibleSections[left][right] <= left)
                continue;
            int rightTime = genes[incompatibleSections[left][right]].getTimeID( );
            if (leftTime == rightTime) {
                if (DEBUG_FITNESS)
                    cout << "Incompatible Time: " << left << " with " << right << endl;
                fitness -= 100;
            }
        }
    }

    //Check for soft constraint for prof time preference.
    for (int secID = 0; secID < gene_length; secID++) {
        int timeAlloted = timeSlots[genes[secID].getTimeID( )]->isMorning( ) ? 0 : (timeSlots[genes[secID].getTimeID( )]->isAfternoon( ) ? 1 : 2);
        fitness -= profPref[genes[secID].getProfID( )][timeAlloted] * 10;
    }

    //Check for soft constraint for section time preference.
    for (int secID = 0; secID < gene_length; secID++) {
        int timeAlloted = timeSlots[genes[secID].getTimeID( )]->isMorning( ) ? 0 : (timeSlots[genes[secID].getTimeID( )]->isAfternoon( ) ? 1 : 2);
        fitness -= sectionPref[secID][timeAlloted];
    }

    if (DEBUG_FITNESS)
        cout << "Current Fitness: " << fitness << endl;
}

string Chromosome::printTable( TimeSlot ** timeSlots, int timeslot_count) {
    const int columnWidth = 12;
    const int hlineWidth = (columnWidth + 1) * 7;
    const char seperator = ' ';
    int hour = 800;
    string rtnVal = "";
    vector<PeriodData> * days = new vector<PeriodData>[6];
    rtnVal += printHLine(hlineWidth); 
    rtnVal += printHeader( ) + "\n";
    char row = 'a' - 1;
    
    for (int i = 0; i < 56; ++i) {
        rtnVal += printHLine(hlineWidth);
        string columnHeader = "";
        if (i % 4 == 0) {
            row++;
            if (hour >= 1000)
                columnHeader = to_string(hour);
            else
                columnHeader = "0" + to_string(hour);
            hour += 100;
        } 

        columnHeader.resize(columnWidth, ' ');
        rtnVal += columnHeader + "|";
        
        string columnVal = "";
        char colChar = 'A' - 1;
        for (int col = 0; col < 6; ++col) {
            columnVal = to_string(i) + ":" + to_string(col) + "# " + row + "%" + ++colChar + "" + to_string((i % 4));
            columnVal.resize(columnWidth, ' ');
            columnVal += "|";
            rtnVal += columnVal;
        }
        
        rtnVal += "\n";
    }
    rtnVal += printHLine(hlineWidth);
    return rtnVal;
}

string Chromosome::printHeader( ) {
    const int columnWidth = 12;
    const char seperator = ' ';

    string rtnVal = "", currVal = "";

    currVal = " Time";
    currVal.resize(columnWidth, ' ');
    rtnVal += currVal + "|";

    currVal = " Monday";
    currVal.resize(columnWidth, ' ');
    rtnVal += currVal + "|";

    currVal = " Tuesday";
    currVal.resize(columnWidth, ' ');
    rtnVal += currVal + "|";

    currVal = " Wednesday";
    currVal.resize(columnWidth, ' ');
    rtnVal += currVal + "|";

    currVal = " Thursday";
    currVal.resize(columnWidth, ' ');
    rtnVal += currVal + "|";

    currVal = " Friday";
    currVal.resize(columnWidth, ' ');
    rtnVal += currVal + "|";

    currVal = " Saturday";
    currVal.resize(columnWidth, ' ');
    rtnVal += currVal + "|";
        
    return rtnVal;
}

string Chromosome::printHLine(int lineWidth) {
    string rtnVal = "";
    rtnVal.resize(lineWidth, '-');
    rtnVal += "\n";
    return rtnVal;
}


bool operator==(Chromosome &ch1, Chromosome &ch2) {
    if (ch1.gene_length != ch2.gene_length)
        return false;

    for (int i = 0; i < ch1.gene_length; i++) {
        if (ch1.genes[i].getProfID( ) != ch2.genes[i].getProfID( ) || ch1.genes[i].getTimeID( ) != ch2.genes[i].getTimeID( ))
            return false;
    }

    return true;
}