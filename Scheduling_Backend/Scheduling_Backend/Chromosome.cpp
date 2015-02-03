/*
 Author: Siddharth Dahiya
 Package: Course and Professor Scheduling (Backend)
 File: Chromosome.cpp
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

#include "Chromosome.h"

Chromosome::Chromosome(int geneLength, int profCount, int timeslotCount, double * profCredsMax) : gene_length(geneLength), prof_count(profCount), timeslot_count(timeslotCount) {
    fitness = 0;
    valid = false;
    genes = new Gene*[geneLength];
    sectionTabooList = new bool *[gene_length];
    for (int i = 0; i < gene_length; ++i) {
        genes[i] = NULL;
        sectionTabooList[i] = new bool[timeslotCount]();
    }

    professorCredits = new double[profCount];
    professorCreditsInitial = new double[profCount];
    for (int i = 0; i < profCount; i++) {
        professorCredits[i] = profCredsMax[i];
        professorCreditsInitial[i] = profCredsMax[i];
    }
}

Chromosome::Chromosome(const Chromosome * other) :
    gene_length(other->gene_length), prof_count(other->prof_count), timeslot_count(other->timeslot_count) {
    valid = other->valid;
    genes = new Gene*[gene_length];
    professorCredits = new double[other->prof_count];
    professorCreditsInitial = new double[other->prof_count];
    sectionTabooList = new bool*[gene_length];
    fitness = other->fitness;
    for (int i = 0; i < gene_length; i++) {
        genes[i] = new Gene(other->genes[i]->getProfID(), other->genes[i]->getTimeID());
        sectionTabooList[i] = new bool[timeslot_count];
        for (int j = 0; j < timeslot_count; ++j) {
            sectionTabooList[i][j] = other->sectionTabooList[i][j];
        }
    }

    for (int i = 0; i < other->prof_count; i++) {
        professorCredits[i] = other->professorCredits[i];
        professorCreditsInitial[i] = other->professorCreditsInitial[i];
    }
}

Chromosome::Chromosome(const Chromosome &source) :
    gene_length(source.gene_length), prof_count(source.prof_count), timeslot_count(source.timeslot_count) {
    valid = source.valid;
    genes = new Gene*[gene_length];
    sectionTabooList = new bool *[gene_length];

    fitness = source.fitness;
    for (int i = 0; i < gene_length; i++) {
        genes[i] = new Gene(source.genes[i]->getProfID(), source.genes[i]->getTimeID());
        sectionTabooList[i] = new bool[timeslot_count];
        for (int j = 0; j < timeslot_count; ++j) {
            sectionTabooList[i][j] = source.sectionTabooList[i][j];
        }
    }

    professorCredits = new double[source.prof_count];
    professorCreditsInitial = new double[source.prof_count];
    for (int i = 0; i < source.prof_count; i++) {
        professorCredits[i] = source.professorCredits[i];
        professorCreditsInitial[i] = source.professorCreditsInitial[i];
    }
}

void Chromosome::updateTabooList(bool ** incompatibleSectionsMatrix) {
    for (int i = 0; i < gene_length; ++i)
        for (int j = 0; j < timeslot_count; ++j) {
            sectionTabooList[i][j] = false;
        }

    for (int first = 0; first < gene_length - 1; ++first) {
        for (int second = first + 1; second < gene_length; ++second) {
            if (incompatibleSectionsMatrix[first][second] || genes[first]->getProfID() == genes[second]->getProfID()) {
                sectionTabooList[first][genes[second]->getTimeID()] = true;
                sectionTabooList[second][genes[first]->getTimeID()] = true;
            }
        }
    }
}

double Chromosome::getCourseLoad(int profID) {
    return professorCredits[profID];
}

string Chromosome::getProfessorLoads() {
    std::ostringstream ss;
    for (int i = 0; i < prof_count; i++) {
        ss << "(" << i << "," << fixed << setprecision(2) << professorCredits[i]
            << ")";
    }
    return ss.str();
}

Chromosome& Chromosome::operator=(const Chromosome &source) {
    genes = new Gene*[gene_length];
    fitness = source.fitness;
    for (int i = 0; i < gene_length; i++) {
        genes[i] = new Gene(source.genes[i]->getProfID(),
            source.genes[i]->getTimeID());
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

Chromosome::~Chromosome() {
    if (genes) {
        for (int i = 0; i < gene_length; ++i) {
            delete genes[i];
        }
        delete[] genes;
    }
    if (professorCredits)
        delete[] professorCredits;
}

bool Chromosome::isValid() {
    return valid;
}

bool Chromosome::isValid(bool ** incompatibleSectionsMatrix, bool ** timeslotConflict, bool verbose) {
    validate(incompatibleSectionsMatrix, timeslotConflict, verbose);
    return valid;
}

bool Chromosome::sortByProfessorCredit(int i, int j) {
    return professorCredits[i] < professorCredits[j];
}

bool Chromosome::equals(const Chromosome * other)
{
    if (fitness != other->fitness)
        return false;
    for (int i = 0; i < gene_length; ++i) {
        if (genes[i]->getProfID() != other->genes[i]->getProfID() || genes[i]->getTimeID() != other->genes[i]->getTimeID())
            return false;
    }
    return true;
}

void Chromosome::setTime(int geneID, int newTime) {
    if (genes[geneID]) {
        genes[geneID]->setTimeID(newTime);
    }
    else {
        genes[geneID] = new Gene(-1, newTime);
    }
}

void Chromosome::setProf(int geneID, int newProf, double courseCred) {
    if (genes[geneID]) {
        genes[geneID]->setProfID(newProf);
    }
    else {
        genes[geneID] = new Gene(newProf, -1);
    }

}

void Chromosome::setGene(int geneID, Gene newGene) {
    if (genes[geneID])
        delete genes[geneID];
    genes[geneID] = new Gene(newGene);
}

const int Chromosome::getTime(int geneID) {
    return genes[geneID]->getTimeID();
}

const int Chromosome::getProf(int geneID) {
    return genes[geneID]->getProfID();
}

Gene* Chromosome::getGene(int geneID) {
    return genes[geneID];
}

int Chromosome::getFitness() {
    return fitness;
}

bool Chromosome::validProfessorLoad(int p) {
    return (professorCredits[p] >= DELTA_MAX && professorCredits[p] <= DELTA_MIN);
}

bool Chromosome::validProfessorLoadChange(int p, double credit) {
    return ((professorCredits[p] - credit) >= DELTA_MAX
        || (professorCredits[p] - credit) <= DELTA_MIN);
}

string Chromosome::print() {
    string rtnVal;
    for (int i = 0; i < gene_length; i++) {
        rtnVal += to_string(i) + "> " + to_string(genes[i]->getProfID()) + ", "
            + to_string(genes[i]->getTimeID()) + "\n";
    }

    rtnVal += "----------------------------\n";

    return rtnVal;
}

string Chromosome::print(int geneID) {
    return to_string(genes[geneID]->getProfID()) + ", "
        + to_string(genes[geneID]->getTimeID());
}

string Chromosome::printTuple() {
    stringstream rtnVal;
    for (int i = 0; i < gene_length; ++i) {
        rtnVal << i << "," << print(i) << endl;
    }

    return rtnVal.str();
}


void Chromosome::evolve(int * const sortedSectionList, int ** const sectionProf, int ** const sectionTimeslot, Helper * const h, int const mutation_probability, double * const sectionCredit, bool ** const incompatibleSectionsMatrix, bool ** const timeslotConflict, int credit_count, int ** const profSection, int *** const associatedProfessors, int ** const sectionPref, int ** const profPref, bool ** const timeslotDaytime, bool ** const timeslotConsecutive, bool ** const timeslotSpread) {
    updateTabooList(incompatibleSectionsMatrix);
    mutate(incompatibleSectionsMatrix, sortedSectionList, sectionProf, sectionTimeslot, h, mutation_probability, sectionCredit);
    repair(sortedSectionList, incompatibleSectionsMatrix, timeslotConflict, sectionCredit, sectionTimeslot, sectionProf, profSection, associatedProfessors);
    updateFitness(incompatibleSectionsMatrix, sectionPref, profPref, timeslotDaytime, timeslotConflict, timeslotConsecutive, timeslotSpread);
    if (isValid())
        optimize(sectionProf, sectionTimeslot, h, sectionCredit, profSection, sectionPref, profPref, incompatibleSectionsMatrix, timeslotDaytime, timeslotConflict, timeslotConsecutive, timeslotSpread);
}

void Chromosome::mutate(bool ** incompatibleSectionsMatrix, int * sortedSectionList, int ** sectionProf, int ** sectionTimeslot, Helper * h, int mutation_probability, double * sectionCredit) {

    if (DEBUG_MUTATION)
        cout << "Mutating Individual... " << endl;

    for (int idx = 0; idx < gene_length; idx++) {
        int g = sortedSectionList[idx];
        int timeID = -1, compTimes = -1, profID = -1, compProfs = -1;

        compProfs = sectionProf[g][0];
        profID = getProf(g);

        compTimes = sectionTimeslot[idx][0];
        timeID = getTime(g);

        int mutationTime = h->randNum(1, 100);
        if (mutationTime <= mutation_probability) {
            if (compTimes != 1) {

                timeID = h->randNum(1, compTimes);
                for (timeID = 1; timeID < compTimes && sectionTabooList[idx][sectionTimeslot[idx][timeID]]; ++timeID);
            }
            else
                timeID = 1;
            setTime(g, sectionTimeslot[idx][timeID]);
            updateTabooList(incompatibleSectionsMatrix);
        }

        int mutationProf = h->randNum(1, 100);
        if (mutationProf <= mutation_probability) {
            if (compProfs != 1) {
                profID = h->randNum(1, compProfs);
                if (profID != getProf(g))
                    setProf(g, sectionProf[g][profID], sectionCredit[g]);
            }
            else
                profID = 1;
        }
    }
    updateProfLoad(sectionCredit);
    if (DEBUG_MUTATION)
        cout << "Finished Mutating." << endl;
}

string Chromosome::shiftSectionToOverloaded(int target, int parentID, int profID, int ** sectionProf, int ** profSection, double * sectionCredit, int *** associatedProfessors) {
    if (DEBUG_BALANCEPROFLOAD)
        cout << "Shift Section with parent: " << parentID << " and prof " << profID << endl;
    visitedProfessors[profID] = true;
    if (professorCredits[profID] > 0) {
    CURRENTPROFUNDERLOADED:
        int profIDIndex = 1;
        for (; profIDIndex <= associatedProfessors[parentID][0][0]; ++profIDIndex) {
            if (profID == associatedProfessors[parentID][profIDIndex][0])
                break;
        }
        for (int sTeach = 2; sTeach <= associatedProfessors[parentID][profIDIndex][1] + 1; ++sTeach) {
            if (genes[associatedProfessors[parentID][profIDIndex][sTeach]]->getProfID() == parentID) {
                //cout << "Returning: " << associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] << endl;
                return to_string(associatedProfessors[parentID][profIDIndex][sTeach]) + "," + to_string(associatedProfessors[parentID][profIDIndex][0]);
            }
            //cout << genes[ associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] ]->getProfID( ) << " is teaching " << associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] << endl;
        }
        if (DEBUG_BALANCEPROFLOAD)
            cout << "Unable to find a shared section between " << parentID << " and prof " << profID << endl;
        if (professorCredits[profID] > 0)
            visitedProfessors[profID] = false;
        return "";
    }

    for (int associateIndex = 1; associateIndex <= associatedProfessors[profID][0][0]; ++associateIndex) {
        if (professorCredits[profID] > 0)
            goto CURRENTPROFUNDERLOADED;
        if (visitedProfessors[associatedProfessors[profID][associateIndex][0]] || professorCredits[associatedProfessors[profID][associateIndex][0]] < 0 || associatedProfessors[profID][associateIndex][0] == target)
            continue;
        bool good = false;
        for (int x = 2; x <= associatedProfessors[profID][associateIndex][1] + 1 && !good; ++x) {
            if (genes[associatedProfessors[profID][associateIndex][x]]->getProfID() == profID)
                good = true;
        }
        if (!good)
            continue;
        string sectionToTransfer = shiftSectionToOverloaded(target, profID, associatedProfessors[profID][associateIndex][0], sectionProf, profSection, sectionCredit, associatedProfessors);
        if (!sectionToTransfer.empty()) {
            vector<string> data = Utility::Tokenize(sectionToTransfer, ',');
            int newProf = stoi(data.at(1));
            int section = stoi(data.at(0));
            genes[section]->setProfID(newProf);
            updateProfLoad(sectionCredit);
            if (professorCredits[newProf] > 0)
                visitedProfessors[newProf] = false;
            int profIDIndex = 1;
            for (; profIDIndex <= associatedProfessors[parentID][0][0]; ++profIDIndex) {
                if (profID == associatedProfessors[parentID][profIDIndex][0])
                    break;
            }
            for (int sTeach = 2; sTeach <= associatedProfessors[parentID][profIDIndex][1] + 1; ++sTeach) {
                if (genes[associatedProfessors[parentID][profIDIndex][sTeach]]->getProfID() == parentID) {
                    //cout << "Returning: " << associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] << endl;
                    //return associatedProfessors[ parentID ][ profIDIndex ][ sTeach ];
                    return to_string(associatedProfessors[parentID][profIDIndex][sTeach]) + "," + to_string(associatedProfessors[parentID][profIDIndex][0]);
                }
                //cout << genes[ associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] ]->getProfID( ) << " is teaching " << associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] << endl;
            }
        }
    }
    return "";
}

string Chromosome::shiftSectionToUnderloaded(int target, int parentID, int profID, int ** sectionProf, int ** profSection, double * sectionCredit, int *** associatedProfessors) {
    if (DEBUG_BALANCEPROFLOAD)
        cout << "Shift Section with parent: " << parentID << " and prof " << profID << endl;
    visitedProfessors[profID] = true;
    if (professorCredits[profID] < 0) {
    CURRENTPROFOVERLOADED:
        int profIDIndex = 1;
        for (; profIDIndex <= associatedProfessors[parentID][0][0]; ++profIDIndex) {
            if (profID == associatedProfessors[parentID][profIDIndex][0])
                break;
        }
        for (int sTeach = 2; sTeach <= associatedProfessors[parentID][profIDIndex][1] + 1; ++sTeach) {
            if (genes[associatedProfessors[parentID][profIDIndex][sTeach]]->getProfID() == profID) {
                //cout << "Returning: " << associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] << endl;
                return to_string(associatedProfessors[parentID][profIDIndex][sTeach]) + "," + to_string(associatedProfessors[parentID][profIDIndex][0]);
            }
            //cout << genes[ associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] ]->getProfID( ) << " is teaching " << associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] << endl;
        }
        if (DEBUG_BALANCEPROFLOAD)
            cout << "Unable to find a shared section between " << parentID << " and prof " << profID << endl;
        if (professorCredits[profID] < 0)
            visitedProfessors[profID] = false;
        return "";
    }

    for (int associateIndex = 1; associateIndex <= associatedProfessors[profID][0][0]; ++associateIndex) {
        if (professorCredits[profID] < 0)
            goto CURRENTPROFOVERLOADED;
        if (visitedProfessors[associatedProfessors[profID][associateIndex][0]] || professorCredits[associatedProfessors[profID][associateIndex][0]] > 0 || associatedProfessors[profID][associateIndex][0] == target)
            continue;
        bool good = false;
        for (int x = 2; x <= associatedProfessors[profID][associateIndex][1] + 1 && !good; ++x) {
            if (genes[associatedProfessors[profID][associateIndex][x]]->getProfID() == associatedProfessors[profID][associateIndex][0])
                good = true;
        }
        if (!good)
            continue;
        string sectionToTransfer = shiftSectionToUnderloaded(target, profID, associatedProfessors[profID][associateIndex][0], sectionProf, profSection, sectionCredit, associatedProfessors);
        if (!sectionToTransfer.empty()) {
            vector<string> data = Utility::Tokenize(sectionToTransfer, ',');
            int newProf = stoi(data.at(1));
            int section = stoi(data.at(0));
            genes[section]->setProfID(profID);
            updateProfLoad(sectionCredit);
            if (professorCredits[newProf] < 0)
                visitedProfessors[newProf] = false;
            int profIDIndex = 1;
            for (; profIDIndex <= associatedProfessors[parentID][0][0]; ++profIDIndex) {
                if (profID == associatedProfessors[parentID][profIDIndex][0])
                    break;
            }
            for (int sTeach = 2; sTeach <= associatedProfessors[parentID][profIDIndex][1] + 1; ++sTeach) {
                if (genes[associatedProfessors[parentID][profIDIndex][sTeach]]->getProfID() == profID) {
                    //cout << "Returning: " << associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] << endl;
                    return to_string(associatedProfessors[parentID][profIDIndex][sTeach]) + "," + to_string(associatedProfessors[parentID][profIDIndex][0]);
                }
                //cout << genes[ associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] ]->getProfID( ) << " is teaching " << associatedProfessors[ parentID ][ profIDIndex ][ sTeach ] << endl;
            }
        }
    }
    return "";
}


void Chromosome::balanceProfLoad(int ** sectionProf, int ** profSection, double * sectionCredit, int *** associatedProfessors)
{
    int prevProfBalancedCount = 0;
    int currentProfBalancedCount = 0;
    int * profBackupList = new int[gene_length];
    do {
        for (int i = 0; i < gene_length; ++i) {
            profBackupList[i] = genes[i]->getProfID();
        }

        prevProfBalancedCount = currentProfBalancedCount;
        currentProfBalancedCount = 0;
        for (int profID = 0; profID < prof_count; ++profID) {
            if (professorCredits[profID] < DELTA_MAX) {
                currentProfBalancedCount++;
                visitedProfessors = new bool[prof_count]();
                visitedProfessors[profID] = true;
                if (DEBUG_BALANCEPROFLOAD)
                    cout << "Starting with " << profID;
                //Go through the associates to find a professor that can transfer classes.
                while (professorCredits[profID] < DELTA_MAX) {
                    int associateIndex = 1;
                    for (; associateIndex <= associatedProfessors[profID][0][0] && professorCredits[profID] < DELTA_MAX; ++associateIndex) {
                        if (DEBUG_BALANCEPROFLOAD)
                            cout << " Associate: " << associatedProfessors[profID][associateIndex][0] << endl;
                        bool good = false;
                        for (int x = 2; x <= associatedProfessors[profID][associateIndex][1] + 1; ++x) {
                            if (genes[associatedProfessors[profID][associateIndex][x]]->getProfID() == profID) {
                                good = true;
                                break;
                            }
                        }
                        if (!good)
                            continue;
                        string sectionToShift = shiftSectionToOverloaded(profID, profID, associatedProfessors[profID][associateIndex][0], sectionProf, profSection, sectionCredit, associatedProfessors);
                        if (sectionToShift.empty())
                            continue;
                        vector<string> data = Utility::Tokenize(sectionToShift, ',');
                        int newProf = stoi(data.at(1));
                        int section = stoi(data.at(0));
                        genes[section]->setProfID(newProf);
                        updateProfLoad(sectionCredit);
                        if (professorCredits[newProf] > 0)
                            visitedProfessors[newProf] = false;

                    }
                    //Check if we went through all profs to try and balance this.
                    if (associateIndex > associatedProfessors[profID][0][0])
                        break;
                }
                delete[] visitedProfessors;
                if (DEBUG_BALANCEPROFLOAD) {
                    if (professorCredits[profID] < DELTA_MAX)
                        cout << "Unable to balance " << profID << " :(" << endl;
                    else
                        cout << profID << " balanced. :)" << endl;
                }
            }
            else if (professorCredits[profID] > DELTA_MIN) {
                currentProfBalancedCount++;
                visitedProfessors = new bool[prof_count]();
                visitedProfessors[profID] = true;
                if (DEBUG_BALANCEPROFLOAD)
                    cout << "Starting with " << profID;

                //Go through the associates to find a professor that can transfer classes.
                while (professorCredits[profID] > DELTA_MIN) {
                    int associateIndex = 1;
                    for (; associateIndex <= associatedProfessors[profID][0][0] && professorCredits[profID] > DELTA_MIN; ++associateIndex) {
                        if (DEBUG_BALANCEPROFLOAD)
                            cout << " Associate: " << associatedProfessors[profID][associateIndex][0] << endl;
                        string sectionToShift = shiftSectionToUnderloaded(profID, profID, associatedProfessors[profID][associateIndex][0], sectionProf, profSection, sectionCredit, associatedProfessors);
                        if (sectionToShift.empty())
                            continue;
                        vector<string> data = Utility::Tokenize(sectionToShift, ',');
                        int newProf = stoi(data.at(1));
                        int section = stoi(data.at(0));
                        genes[section]->setProfID(profID);
                        updateProfLoad(sectionCredit);
                        if (professorCredits[newProf] < 0)
                            visitedProfessors[newProf] = false;
                    }
                    //Check if we went through all profs to try and balance this.
                    if (associateIndex > associatedProfessors[profID][0][0])
                        break;
                }
                delete[] visitedProfessors;
                if (DEBUG_BALANCEPROFLOAD) {
                    if (professorCredits[profID] > DELTA_MIN)
                        cout << "Unable to balance " << profID << " :(" << endl;
                    else
                        cout << profID << " balanced. :)" << endl;
                }
            }
        }
    } while (prevProfBalancedCount > currentProfBalancedCount && currentProfBalancedCount != 0);
    if (currentProfBalancedCount > prevProfBalancedCount) {
        for (int i = 0; i < gene_length; ++i) {
            genes[i]->setProfID(profBackupList[i]);
        }
    }
    delete[] profBackupList;
}

bool Chromosome::professorsBalanced() {
    for (int i = 0; i < prof_count; ++i) {
        if (!(professorCredits[i] >= DELTA_MAX && professorCredits[i] <= DELTA_MIN))
            return false;
    }
    return true;
}

void Chromosome::repair(int * sortedSectionList, bool ** incompatibleSectionsMatrix, bool ** timeslotConflict, double * sectionCredit, int ** sectionTimeslot, int ** sectionProf, int ** profSection, int *** associatedProfessors) {
    int retry = 0;
    do {
        balanceProfLoad(sectionProf, profSection, sectionCredit, associatedProfessors);
        bool ** sectionTimeTabooList = new bool*[gene_length];
        for (int i = 0; i < gene_length; ++i) {
            sectionTimeTabooList[i] = new bool[timeslot_count]();
        }
        for (int index = 0; index < gene_length; ++index) {
            int currentSection = sortedSectionList[index];
            for (int otherSection = 0; otherSection < gene_length; ++otherSection) {
                if (currentSection == otherSection)
                    continue;
                if (incompatibleSectionsMatrix[currentSection][otherSection] || genes[currentSection]->getProfID() == genes[otherSection]->getProfID()) {
                    if (timeslotConflict[genes[currentSection]->getTimeID()][genes[otherSection]->getTimeID()]) {
                        sectionTimeTabooList[currentSection][genes[currentSection]->getTimeID()] = true;
                        int potentialTimeslotIndex = 1;

                        for (; potentialTimeslotIndex <= sectionTimeslot[currentSection][0] && sectionTimeTabooList[currentSection][sectionTimeslot[currentSection][potentialTimeslotIndex]]; ++potentialTimeslotIndex) {
                            ;
                        }
                        genes[currentSection]->setTimeID(sectionTimeslot[currentSection][potentialTimeslotIndex]);

                    }
                }
            }
        }
        for (int i = 0; i < gene_length; ++i) {
            delete[] sectionTimeTabooList[i];
        }
        delete[] sectionTimeTabooList;
        validate(incompatibleSectionsMatrix, timeslotConflict, false);
    } while (!valid && ++retry < REPAIR_MAX);
}

void Chromosome::optimize(int ** sectionProf, int ** sectionTimeslot, Helper * h, double * sectionCredit, int ** profSection, int ** sectionPref, int ** profPref, bool ** incompatibleSectionsMatrix, bool ** timeslotDaytime, bool ** timeslotConflict, bool ** timeslotConsecutive, bool ** timeslotSpread) {
    /*
    Idea:
    1. Swap 2 faculty.
    2. Check fitness function.
    3. If fitness has improved, repeat again with 2 more faculty.
    4. Repeat until fitness stops improving.
    */

    int startingFitness = fitness;
    int sectionIdx = 0;
    int lastProfLeft;
    int lastProfRight;
    int lastIndexLeft;
    int lastIndexRight;

    /*
     Find 2 faculty to swap by going through the sections
     and finding first such section which has two professors.
     Ensure when swapping that the schedule is not invalidated.
     */
    bool swapSuccess = false;
    for (; sectionIdx < gene_length; ++sectionIdx) {
        //Does the current section have more than two profs associated?
        if (sectionProf[sectionIdx][0] > 1) {
            //Find one professor who is not teaching this course
            int currentProf = getProf(sectionIdx);
            int profToSwapWith = -1;
            for (int nextProf = 1; nextProf <= sectionProf[sectionIdx][0]; ++nextProf) {
                if (sectionProf[sectionIdx][nextProf] != currentProf) {
                    profToSwapWith = sectionProf[sectionIdx][nextProf];
                    break;
                }
            }

            //Now find a section that the other prof is teaching to swap with,
            //such that the chromosome is not invalidated
            for (int newSectionIdx = 0;
            newSectionIdx < gene_length && !swapSuccess;
                ++newSectionIdx) {
                if (getProf(newSectionIdx) == profToSwapWith) {
                    //Can the currentProf teach this section
                    for (int canTeach = 1; canTeach <= sectionProf[newSectionIdx][0] && !swapSuccess; ++canTeach) {
                        if (sectionProf[newSectionIdx][canTeach]
                            == currentProf) {
                            //Perform swap
                            setProf(sectionIdx, profToSwapWith, sectionCredit[sectionIdx]);
                            setProf(newSectionIdx, currentProf, sectionCredit[newSectionIdx]);
                            //check valid
                            validate(incompatibleSectionsMatrix, timeslotConflict, false);
                            updateFitness(incompatibleSectionsMatrix, sectionPref, profPref, timeslotDaytime, timeslotConflict, timeslotConsecutive, timeslotSpread);
                            //Check if swap was valid and it is better fitness
                            if (!valid && fitness > startingFitness) {
                                //Undo the swap
                                setProf(sectionIdx, currentProf, sectionCredit[sectionIdx]);
                                setProf(newSectionIdx, profToSwapWith, sectionCredit[newSectionIdx]);
                                continue;
                            }
                            else {
                                swapSuccess = true;
                                lastIndexLeft = sectionIdx;
                                lastIndexRight = newSectionIdx;
                                lastProfLeft = currentProf;
                                lastProfRight = profToSwapWith;
                                startingFitness = fitness;
                            }
                        } //if(sectionProf[ newSectionIdx ][ canTeach ] == currentProf)
                    } //for(int canTeach = 1; canTeach <= sectionProf[ newSectionIdx ][ 0 ] && !swapSuccess; ++canTeach)
                }                 //if(getProf(newSectionIdx) == profToSwapWith)
            } // for(int newSectionIdx = 0; newSectionIdx < gene_length && !swapSuccess; ++newSectionIdx)
        } //if(sectionProf[ sectionIdx ][ 0 ] > 1)
    } //for(; sectionIdx < gene_length; ++sectionIdx)
}

void Chromosome::validate(bool ** incompatibleSectionsMatrix, bool ** timeslotConflict, bool verbose) {
    valid = true;
    bool localValid = true;
    /*
     First, I will try to ensure that a professor is not scheduled twice at the same timeID.
     Then, I will look to ensure that no incompatible classes are scheduled at the same timeID.
     */

    for (int pr = 0; pr < prof_count && valid; pr++) {
        if (DEBUG_VALIDATE) {
            cout << "Prof " << pr << " is with " << getCourseLoad(pr);
        }
        if (!(getCourseLoad(pr) >= DELTA_MAX && getCourseLoad(pr) <= DELTA_MIN)) {
            if (verbose) {
                cout << pr << " UNBALANCED with " << getCourseLoad(pr) << endl;
            }
            valid = false;
            localValid = false;
        }
        else {
            if (DEBUG_VALIDATE) {
                cout << endl;
            }
        }
    }

    for (int left = 0; left < gene_length && valid; ++left) {
        int leftProf = getProf(left);
        for (int right = left + 1; right < gene_length && valid; ++right) {
            int rightProf = getProf(right);
            if (leftProf == rightProf && timeslotConflict[getTime(left)][getTime(right)]) {
                //int leftTime = getTime(left), rightTime = getTime(right);
                //if(timeslotConflict[ leftTime ][ rightTime ]) {//timeSlots[ leftTime ]->isOverlap(timeSlots[ rightTime ])) {
                if (verbose)
                    cout << "Incompatible Prof: " << left << " with " << right << endl;
                valid = false;
                localValid = false;
                break;
                //}
            }
        }
    }



    for (int left = 0; left < gene_length && valid; ++left) {
        int leftTime = getTime(left);
        for (int right = left + 1; right < gene_length && valid; ++right) {
            if (!incompatibleSectionsMatrix[left][right])
                continue;
            int rightTime = getTime(right);
            if (leftTime == rightTime) {
                if (verbose)
                    cout << "Incompatible Sections: " << left << " with " << right - 1 << endl;
                valid = false;
                localValid = false;
                break;
            }
        }
    }

    if (verbose) {
        cout << "Validation result: " << localValid << endl;
    }
}

void Chromosome::updateFitness(bool ** incompatibleSectionsMatrix, int ** sectionPref, int ** profPref, bool ** timeslotDaytime, bool ** timeslotConflict, bool ** timeslotConsecutive, bool ** timeslotSpread) {
    fitness = MAXFITNESS;
    bool * profConflict = new bool[gene_length];
    int numConflicts = 0;
    vector<int> * professorTimeSlotsAssigned = new vector<int>[prof_count];
    //Check for hard constraint violation for professor
    for (int left = 0; left < gene_length; left++) {
        if (left < 0)
            cout << "Inside loop: " << gene_length;
        int leftProf = genes[left]->getProfID();
        professorTimeSlotsAssigned[leftProf].push_back(genes[left]->getTimeID());
        for (int right = left + 1; right < gene_length; right++) {
            int rightProf = genes[right]->getProfID();
            if (leftProf == rightProf) {
                int leftTime = genes[left]->getTimeID(), rightTime = genes[right]->getTimeID();
                if (leftTime == rightTime || timeslotConflict[leftTime][rightTime]) {// timeSlots[ leftTime ]->isOverlap(timeSlots[ rightTime ])) {
                    if (DEBUG_FITNESS)
                        cout << "Incompatible Prof Time: " << left << " with " << right << endl;
                    //Create penalty based on Max_Fitness divided by professor count.
                    fitness -= PENALTY_PROFESSOR_TIME_CONFLICT;
                    profConflict[left] = true;
                }
                else
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
            if (professorCredits[p] >= DELTA_MAX && professorCredits[p] <= DELTA_MIN) {
                fitness -= PENALTY_PROFESSOR_LOAD_MINOR;
            }
            else {
                fitness -= PENALTY_PROFESSOR_LOAD_MAJOR;
            }
        }
    }

    //Check for hard constraint violation for timeID conflict
    for (int left = 0; left < gene_length; left++) {
        if (left < 0)
            cout << "Inside loop2: " << gene_length;
        int leftTime = genes[left]->getTimeID();
        for (int right = left + 1; right < gene_length; ++right) {
            if (incompatibleSectionsMatrix[left][right] && genes[right]->getTimeID() == leftTime) {
                if (DEBUG_FITNESS)
                    cout << "Incompatible Time: " << left << " with " << right << endl;
                fitness -= PENALTY_SECTION_TIME_CONFLICT;
            }
        }
    }

    //Check for soft constraint for prof timeID preference.
    for (int secID = 0; secID < gene_length; secID++) {
        int timeAlloted = timeslotDaytime[genes[secID]->getTimeID()][0] ? 0 : (timeslotDaytime[genes[secID]->getTimeID()][1] ? 1 : 2);

        if (DEBUG_FITNESS) {
            ; //cout << "Allotment: " << profPref[genes[secID].getProfID( )][timeAlloted] << " Penalty: " << MAX_FITNESS << "/" << pow(prof_count + gene_length, 2.0) << " = " << MAX_FITNESS / pow((prof_count + gene_length), 2.0) << endl;
        }
        fitness -= profPref[genes[secID]->getProfID()][timeAlloted] * PENALTY_PROFESSOR_PREFERENCE;

    }

    //Check for soft constraint for section timeID preference.
    for (int secID = 0; secID < gene_length; secID++) {
        int timeAlloted = timeslotDaytime[genes[secID]->getTimeID()][0] ? 0 : (timeslotDaytime[genes[secID]->getTimeID()][1] ? 1 : 2);
        fitness -= sectionPref[secID][timeAlloted] * PENALTY_SECTION_PREFERENCE;
    }

    //Check for soft constraint for more than 2 consecutive classes or spread out classes.
    for (int profIndex = 0; profIndex < prof_count; ++profIndex) {
        for (int first = 0; first < (signed)professorTimeSlotsAssigned[profIndex].size(); ++first) {
            for (int second = first + 1; second < (signed)professorTimeSlotsAssigned[profIndex].size(); ++second) {
                if (timeslotConsecutive[first][second]) { // timeSlots[ first ]->isConsecutive(timeSlots[ second ])) {
                    //Two consecutive timeslots, look for a third.
                    for (int third = second + 1; third < (signed)professorTimeSlotsAssigned[profIndex].size(); ++third) {
                        if (timeslotConsecutive[second][third]) { //timeSlots[ second ]->isConsecutive(timeSlots[ third ])) {
                            //Bingo! Three consecutive timeslots.
                            fitness -= PENATLY_PROFESSOR_CONSECUTIVE_TIMES;
                        }
                    }
                }
                //Check for soft constraint for disjoint classes for the professor (morning and evening, no afternoon)
                else if (timeslotSpread[first][second]) { //timeSlots[ first ]->isSpreadOut(timeSlots[ second ])) {
                    fitness -= PENATLY_PROFESSOR_SPREADOUT_TIMES;
                }
            }
        }
    }

    delete[] professorTimeSlotsAssigned;
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

string Chromosome::printProfTable() {
    ostringstream ss;
    string * profs = new string[prof_count];
    for (int i = 0; i < prof_count; ++i) {
        profs[i] = (i < 10 ? " " : "") + to_string(i) + " ["
            + (professorCredits[i] < 0 ? "" : " ")
            + Utility::FormatDouble(professorCredits[i], 2) + "]: ";
        if (professorCredits[i] < DELTA_MAX || professorCredits[i] > DELTA_MIN)
            profs[i] = "# " + profs[i];
        else
            profs[i] = "  " + profs[i];
    }
    for (int i = 0; i < gene_length; ++i) {
        profs[getProf(i)] += to_string(i) + ", ";
    }
    for (int i = 0; i < prof_count; ++i) {
        ss << profs[i] << endl;
    }
    delete[] profs;
    return ss.str();
}

int Chromosome::DifferenceCount(Chromosome * const parent) {
    int diffs = 0;
    for (int i = 0; i < gene_length; ++i) {
        if (genes[i]->getProfID() != parent->genes[i]->getProfID()) {
            diffs++;
        }
        if (genes[i]->getTimeID() != parent->genes[i]->getTimeID()) {
            diffs++;
        }
    }

    return diffs;
}

bool operator==(Chromosome &ch1, Chromosome &ch2) {
    if (ch1.gene_length != ch2.gene_length)
        return false;

    for (int i = 0; i < ch1.gene_length; i++) {
        if (ch1.genes[i]->getProfID() != ch2.genes[i]->getProfID()
            || ch1.genes[i]->getTimeID() != ch2.genes[i]->getTimeID())
            return false;
    }

    return true;
}
