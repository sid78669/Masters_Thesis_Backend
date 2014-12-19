#include "Chromosome.h"

Chromosome::Chromosome(int geneLength, int profCount, double * profCredsMax) : gene_length(geneLength), prof_count(profCount) {
    fitness = 0;
    genes = new Gene*[geneLength];
    for (int i = 0; i < gene_length; ++i) {
        genes[i] = NULL;
    }

    professorCredits = new double[profCount];
    professorCreditsInitial = new double[profCount];
    for (int i = 0; i < profCount; i++) {
        professorCredits[i] = profCredsMax[i];
        professorCreditsInitial[i] = profCredsMax[i];
    }
}

Chromosome::Chromosome(const Chromosome * other) : gene_length(other->gene_length), prof_count(other->prof_count) {

    genes = new Gene*[gene_length];
    professorCredits = new double[other->prof_count];
    professorCreditsInitial = new double[other->prof_count];
    fitness = other->fitness;
    for (int i = 0; i < gene_length; i++) {
        genes[i] = new Gene(other->genes[i]->getProfID( ), other->genes[i]->getTimeID( ));
    }

    for (int i = 0; i < other->prof_count; i++) {
        professorCredits[i] = other->professorCredits[i];
        professorCreditsInitial[i] = other->professorCreditsInitial[i];
    }
}

Chromosome::Chromosome(const Chromosome &source) : gene_length(source.gene_length), prof_count(source.prof_count) {
    cout << "Overloaded Copy Constructor" << endl;

    genes = new Gene*[gene_length];

    fitness = source.fitness;
    for (int i = 0; i < gene_length; i++) {
        genes[i] = new Gene(source.genes[i]->getProfID( ),
                            source.genes[i]->getTimeID( ));
    }

    professorCredits = new double[source.prof_count];
    professorCreditsInitial = new double[source.prof_count];
    for (int i = 0; i < source.prof_count; i++) {
        professorCredits[i] = source.professorCredits[i];
        professorCreditsInitial[i] = source.professorCreditsInitial[i];
    }
}

double Chromosome::getCourseLoad(int profID) {
    return professorCredits[profID];
}

string Chromosome::getProfessorLoads( ) {
    std::ostringstream ss;
    for (int i = 0; i < prof_count; i++) {
        ss << "(" << i << "," << fixed << setprecision(2) << professorCredits[i] << ")";
    }
    return ss.str( );
}

Chromosome& Chromosome::operator=(const Chromosome &source) {
    genes = new Gene*[gene_length];
    fitness = source.fitness;
    for (int i = 0; i < gene_length; i++) {
        genes[i] = new Gene(source.genes[i]->getProfID( ), source.genes[i]->getTimeID( ));
    }

    for (int i = 0; i < source.prof_count; i++) {
        professorCredits[i] = source.professorCredits[i];
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
    if (genes) {
        for (int i = 0; i < gene_length; ++i) {
            delete genes[i];
        }
        delete[] genes;
    }
    if (professorCredits)
        delete[] professorCredits;
}

void Chromosome::setTime(int geneID, int newTime) {
    if (genes[geneID]) {
        genes[geneID]->setTimeID(newTime);
    } else {
        genes[geneID] = new Gene(-1, newTime);
    }
}

void Chromosome::setProf(int geneID, int newProf, double courseCred) {
    if (genes[geneID]) {
        //professorCredits[getProf(geneID)] += courseCred;
        //professorCredits[newProf] -= courseCred;
        genes[geneID]->setProfID(newProf);
    } else {
        genes[geneID] = new Gene(newProf, -1);
        //professorCredits[newProf] -= courseCred;
    }

}

void Chromosome::setGene(int geneID, Gene newGene) {
    if (genes[geneID])
        delete genes[geneID];
    genes[geneID] = new Gene(newGene);
}

const int Chromosome::getTime(int geneID) {
    return genes[geneID]->getTimeID( );
}

const int Chromosome::getProf(int geneID) {
    return genes[geneID]->getProfID( );
}

Gene* Chromosome::getGene(int geneID) {
    return genes[geneID];
}

int Chromosome::getFitness( ) {
    return fitness;
}

bool Chromosome::validProfessorLoad(int p) {
    return (professorCredits[p] >= DELTA_MAX && professorCredits[p] <= DELTA_MIN);
}

bool Chromosome::validProfessorLoadChange(int p, double credit) {
    return ((professorCredits[p] - credit) >= DELTA_MAX || (professorCredits[p] - credit) <= DELTA_MIN);
}

string Chromosome::print( ) {
    string rtnVal;
    for (int i = 0; i < gene_length; i++) {
        rtnVal += to_string(i) + "> " + to_string(genes[i]->getProfID( )) + ", "
            + to_string(genes[i]->getTimeID( )) + "\n";
    }

    rtnVal += "----------------------------\n";

    return rtnVal;
}

string Chromosome::print(int geneID) {
    return to_string(genes[geneID]->getProfID( )) + ", "
        + to_string(genes[geneID]->getTimeID( ));
}

void Chromosome::mutate(int ** sectionProf, int section_count, int ** creditTimeSlot, TimeSlot ** timeSlots, double * timeCredLegend,
                        int timeCredLegendSize, Helper * h, double mutation_probability, double * sectionCredit) {

    if (DEBUG_MUTATION)
        cout << "Mutating Individual... " << endl;

    for (int g = 0; g < section_count; g++) {
        int creditRow = 0, timeID = -1, compTimes = -1, profID = -1, compProfs = -1;

        compProfs = sectionProf[g][0];
        profID = getProf(g);

        while (creditRow < timeCredLegendSize && timeCredLegend[creditRow] != timeSlots[getTime(g)]->getCredits( ))
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
            if (compProfs != 1) {
                //int max_tries = compProfs + 10;
                //do {
                profID = h->randNum(1, compProfs);
                //    max_tries--;
                //    if (max_tries < 0)
                //       break;
                //} while (!(professorCredits[profID] - sectionCredit[g] < DELTA_MAX  || (professorCredits[profID] - sectionCredit[g]) > DELTA_MIN));
                if (profID != getProf(g))
                    setProf(g, sectionProf[g][profID], sectionCredit[g]);
            } else
                profID = 1;
        }
    }
    updateProfLoad(sectionCredit);
    if (DEBUG_MUTATION)
        cout << "Finished Mutating." << endl;
}

void Chromosome::repair(int ** sectionProf, int section_count, int ** creditTimeSlot, TimeSlot ** timeSlots, double * timeCredLegend,
                        int timeCredLegendSize, Helper * h, int ** incompatibleSections, const int REPAIR_TRIES, double * sectionCred, int ** profSection) {
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
            for (int conflictSection = 1; conflictSection <= incompatibleSections[secIndex][0]; ++conflictSection) {

                int right = incompatibleSections[secIndex][conflictSection];

                //We want to ensure that this is a look ahead search. Hence, if the conflicting section is
                //On the left of the current section, skip this iteration.
                if (right <= secIndex)
                    continue;

                //Check if time and prof conflict. In that case, switch prof
                if (genes[secIndex]->getTimeID( ) == genes[right]->getTimeID( ) && genes[secIndex]->getProfID( ) == genes[right]->getProfID( )) {
                    if (DEBUG_REPAIR)
                        cout << "Time and Prof Collision for " << genes[secIndex]->getTimeID( ) << " and " << genes[right]->getTimeID( ) << endl;

                    currentIterationRepaired = true;

                    //Add the professor to the prof taboo list for the conflicting section
                    tabooProf[right].push_back(genes[right]->getTimeID( ));

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
                        if (find(tabooProf[right].begin( ), tabooProf[right].end( ), profID) == tabooProf[right].end( )) {
                            //If the prof is not in the taboo list, set it as the new prof and break out of the loop.
                            setProf(right, profID, sectionCred[right]);
                            break;
                        }
                    } while (--looped >= 0);

                } else if (genes[secIndex]->getTimeID( ) == genes[right]->getTimeID( )) {
                    //Add the current time to the taboo list for time for the conflicting section
                    tabooTime[right].push_back(genes[right]->getTimeID( ));

                    int timeRow = 0;

                    while (timeRow < timeCredLegendSize&& timeCredLegend[timeRow] != timeSlots[genes[right]->getTimeID( )]->getCredits( ))
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
                        if (find(tabooTime[right].begin( ), tabooTime[right].end( ), timeID) == tabooTime[right].end( )) {
                            genes[right]->setTimeID(timeID);
                            break;
                        }

                    } while (--looped >= 0);
                }
            }
        }

        for (int profIndex = 0; profIndex < prof_count; profIndex++) {
            //Check if the professor is overloaded .
            int professorIteration = REPAIR_TRIES;
            while (professorCredits[profIndex] < DELTA_MAX && professorIteration > 0) {
                professorIteration--;
                //Create a list of all courses being taught by the professor
                currentIterationRepaired = true;
                //Array cannot have more sections than the total sections we have.
                int * sectionsTaughtByProf = new int[section_count];
                int stbpCount = 0; //Section taught by professor
                for (int section = 0; section < section_count; ++section) {
                    if (genes[section]->getProfID( ) == profIndex) {
                        sectionsTaughtByProf[stbpCount] = section;
                    }
                }
                //Find a professors that can replace courses till this professor has acceptable course load.
                for (int stbpReplaceIndex = 0; stbpReplaceIndex < stbpCount && !validProfessorLoad(profIndex); ++stbpReplaceIndex) {
                    //section we are modifying
                    int sectionModified = sectionsTaughtByProf[stbpReplaceIndex];
                    //if more than one professor can teach this course, then replace.
                    if (sectionProf[sectionModified][0] >= 2) {
                        int compProf = sectionProf[sectionModified][0];
                        compProf = h->randNum(1, compProf);
                        if (find(tabooProf[sectionModified].begin( ), tabooProf[sectionModified].end( ), compProf) == tabooProf[sectionModified].end( )
                            && validProfessorLoadChange(compProf, sectionCred[sectionModified])) {
                            setProf(sectionModified, compProf, sectionCred[sectionModified]);
                            tabooProf[sectionModified].push_back(profIndex);
                            break;
                        }
                    }
                    updateProfLoad(sectionCred);
                }
                updateProfLoad(sectionCred);
                delete[] sectionsTaughtByProf;
            }

            //If a professor is underloaded, then find a section that is assigned an overloaded course and switch with them.
            professorIteration = REPAIR_TRIES;
            while (professorCredits[profIndex] > DELTA_MIN && professorIteration > 0) {
                professorIteration--;
                //go through the profSection list for the current prof
                for (int currentTaught = 1; currentTaught <= profSection[profIndex][0]; ++currentTaught) {
                    //check if the current prof is overloaded
                    int potentialReplacement = getProf(profSection[profIndex][currentTaught]);
                    if (professorCredits[potentialReplacement] < DELTA_MAX) {
                        //We chose 0 instead of DELTA_MAX because the closer a professor has to the correct course load, the better.
                        if ((professorCredits[potentialReplacement] - sectionCred[profSection[profIndex][currentTaught]]) >= DELTA_MAX) {
                            setProf(profSection[profIndex][currentTaught], potentialReplacement, sectionCred[profSection[profIndex][currentTaught]]);
                        }
                    }
                    updateProfLoad(sectionCred);
                }
                updateProfLoad(sectionCred);
            }
        }

        if (tries > 0 && DEBUG_REPAIR)
            cout << "Try " << tries << ": Any Repairs? "
            << currentIterationRepaired << endl;
        updateProfLoad(sectionCred);
    } while (++tries < REPAIR_TRIES && currentIterationRepaired);

    delete[] tabooProf;
    delete[] tabooTime;

    if (DEBUG_REPAIR)
        cout << "Gene Repaired to the best of the abilities..." << endl;
}

void Chromosome::updateFitness(int ** incompatibleSections, int ** sectionPref,
                               int ** profPref, TimeSlot ** timeSlots, int prof_count,
                               int timeslot_count, int ** profSectionsTaught) {
    fitness = MAX_FITNESS;
    bool * profConflict = new bool[gene_length];
    int numConflicts = 0;
    //Check for hard constraint violation for professor
    for (int left = 0; left < gene_length; left++) {
        if (left < 0)
            cout << "Inside loop: " << gene_length;
        int leftProf = genes[left]->getProfID( );
        for (int right = left + 1; right < gene_length; right++) {
            int rightProf = genes[right]->getProfID( );
            if (leftProf == rightProf) {
                int leftTime = genes[left]->getTimeID( ), rightTime = genes[right]->getTimeID( );
                if (leftTime == rightTime) {
                    if (DEBUG_FITNESS)
                        cout << "Incompatible Prof Time: " << left << " with " << right << endl;
                    //Create penalty based on Max_Fitness divided by professor count.
                    int penalty = MAX_FITNESS / (prof_count);
                    //Increase the penalty based on the number of sections being taught by the prof.
                    if (profSectionsTaught[leftProf][0] >((gene_length * 1.0) / prof_count))
                        penalty *= profSectionsTaught[leftProf][0];
                    numConflicts++;
                    fitness -= penalty;
                    profConflict[left] = true;
                } else
                    profConflict[left] = false;
            }
        }
    }

    //Check to ensure no prof has been assigned more than his share of courses
    for (int p = 0; p < prof_count; p++) {
        if (professorCredits[p] != 0) {
            if (DEBUG_FITNESS) {
                cout << "Professor Overload: " << p << endl;
            }
            fitness -= MAX_FITNESS / prof_count;
        }
    }

    //Check for hard constraint violation for time conflict
    for (int left = 0; left < gene_length; left++) {
        if (left < 0)
            cout << "Inside loop2: " << gene_length;
        int leftTime = genes[left]->getTimeID( );
        for (int right = 1; right <= incompatibleSections[left][0]; right++) {
            if (incompatibleSections[left][right] <= left)
                continue;
            int rightTime =
                genes[incompatibleSections[left][right]]->getTimeID( );
            if (leftTime == rightTime) {
                if (DEBUG_FITNESS)
                    cout << "Incompatible Time: " << left << " with " << right
                    << endl;
                if (!profConflict[left]) {
                    int penalty = MAX_FITNESS / (gene_length);
                    if (incompatibleSections[left][0] > 2)
                        penalty *= incompatibleSections[left][0];
                    numConflicts++;
                    fitness -= penalty;
                } else
                    fitness -= MAX_FITNESS;
            }
        }
    }
    fitness -= (numConflicts * (prof_count + gene_length));
    //Check for soft constraint for prof time preference.
    for (int secID = 0; secID < gene_length; secID++) {
        if (timeSlots[genes[secID]->getTimeID( )] != NULL) {
            int timeAlloted = timeSlots[genes[secID]->getTimeID( )]->isMorning( ) ? 0 : (timeSlots[genes[secID]->getTimeID( )]->isAfternoon( ) ? 1 : 2);

            if (DEBUG_FITNESS) {
                ; //cout << "Allotment: " << profPref[genes[secID].getProfID( )][timeAlloted] << " Penalty: " << MAX_FITNESS << "/" << pow(prof_count + gene_length, 2.0) << " = " << MAX_FITNESS / pow((prof_count + gene_length), 2.0) << endl;
            }
            fitness -= profPref[genes[secID]->getProfID( )][timeAlloted] * MAX_FITNESS / (int) pow((prof_count + gene_length), 2.0);
        } else {
            fitness -= MAX_FITNESS;
        }
    }

    //Check for soft constraint for section time preference.
    for (int secID = 0; secID < gene_length; secID++) {
        int timeAlloted = timeSlots[genes[secID]->getTimeID( )]->isMorning( ) ? 0 : (timeSlots[genes[secID]->getTimeID( )]->isAfternoon( ) ? 1 : 2);
        fitness -= sectionPref[secID][timeAlloted] * MAX_FITNESS / (int) pow((prof_count + gene_length), 2.0);
    }

    if (DEBUG_FITNESS)
        cout << "Current Fitness: " << fitness << endl;
}

void Chromosome::updateProfLoad(double * sectionCredit) {
    //Reset all professor credit values
    for (int prof = 0; prof < prof_count; ++prof) {
        professorCredits[prof] = professorCreditsInitial[prof];
    }

    //Iterate through all sections and update the associated prof credit
    for (int section = 0; section < gene_length; ++section) {
        professorCredits[getProf(section)] -= sectionCredit[section];
    }
}

string Chromosome::printTable(TimeSlot ** timeSlots, int timeslot_count, double * sectionCredit = nullptr) {
    ostringstream ss;
    for (int i = 0; i < gene_length; i++) {
        string sectionIDString = to_string(i);
        sectionIDString.resize(5, ' ');
        string profIDString = "(" + to_string(genes[i]->getProfID( )) + ")";
        profIDString.resize(5, ' ');
        if (sectionCredit)
            ss << "[" << fixed << setprecision(2) << sectionCredit[i] << "]\t";
        ss << sectionIDString << profIDString << ": " << timeSlots[genes[i]->getTimeID( )]->print( ) << endl;
    }

    ss << "-------------------------------------------------------\n";

    return ss.str( );
}

string Chromosome::printTable(TimeSlot ** timeSlots, int timeslot_count) {
    ostringstream ss;
    for (int i = 0; i < gene_length; i++) {
        string sectionIDString = to_string(i);
        sectionIDString.resize(5, ' ');
        string profIDString = "(" + to_string(genes[i]->getProfID( )) + ")";
        profIDString.resize(5, ' ');
        ss << sectionIDString << profIDString << ": " << timeSlots[genes[i]->getTimeID( )]->print( ) << endl;
    }

    ss << "-------------------------------------------------------\n";

    return ss.str( );
}

string Chromosome::printProfTable( ) {
    ostringstream ss;
    string * profs = new string[prof_count]( );
    for (int i = 0; i < gene_length; ++i) {
        profs[getProf(i)] += i + ", ";
    }
    for (int i = 0; i < prof_count; ++i) {
        ss << i << ": " << profs[i] << "[" << professorCredits[i] << "]" << endl;
    }

    return ss.str( );
}

bool operator==(Chromosome &ch1, Chromosome &ch2) {
    if (ch1.gene_length != ch2.gene_length)
        return false;

    for (int i = 0; i < ch1.gene_length; i++) {
        if (ch1.genes[i]->getProfID( ) != ch2.genes[i]->getProfID( )
            || ch1.genes[i]->getTimeID( ) != ch2.genes[i]->getTimeID( ))
            return false;
    }

    return true;
}
