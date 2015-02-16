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

/// <summary>
/// Initializes a new instance of the <see cref="Chromosome"/> class.
/// </summary>
/// <param name="sectionCount">Number fo sections.</param>
/// <param name="profCount">The prof count.</param>
/// <param name="timeslotCount">The timeslot count.</param>
/// <param name="profCredsMax">The prof creds maximum.</param>
/// <param name="section_credit">The section_credit.</param>
Chromosome::Chromosome(int sectionCount, int profCount, int timeslotCount, double * profCredsMax, double * section_credit) :
        section_count(sectionCount), prof_count(profCount), timeslot_count(timeslotCount), sectionCredit(section_credit) {
    fitness = 0;
    valid = false;
    sections = new Gene*[sectionCount];
    sectionTabooList = new bool *[section_count];
    for (int i = 0; i < section_count; ++i) {
        sections[i] = new Gene(-1, -1);
        sectionTabooList[i] = new bool[timeslotCount]();
    }

    professorCredits = new double[profCount];
    professorCreditsInitial = profCredsMax;
    for (int i = 0; i < profCount; i++) {
        professorCredits[i] = profCredsMax[i];
    }
}

/// <summary>
/// Initializes a new instance of the <see cref="Chromosome"/> class.
/// </summary>
/// <param name="source">The source chromosome.</param>
Chromosome::Chromosome(Chromosome * source) :
        section_count(source->section_count), prof_count(source->prof_count), timeslot_count(source->timeslot_count) {
    sectionCredit = source->sectionCredit;
    valid = source->valid;
    sections = new Gene*[section_count];
    professorCredits = new double[source->prof_count];
    professorCreditsInitial = source->professorCreditsInitial;
    sectionTabooList = new bool*[section_count];
    fitness = source->fitness;
    for (int i = 0; i < section_count; i++) {
        sections[i] = new Gene(source->getProf(i), source->getTime(i));
        sectionTabooList[i] = new bool[timeslot_count];
        for (int j = 0; j < timeslot_count; ++j) {
            sectionTabooList[i][j] = source->sectionTabooList[i][j];
        }
    }

    for (int i = 0; i < source->prof_count; i++) {
        professorCredits[i] = source->professorCredits[i];
    }
}

/// <summary>
/// Initializes a new instance of the <see cref="Chromosome"/> class.
/// </summary>
/// <param name="source">The source chromosome.</param>
Chromosome::Chromosome(Chromosome &source) :
        section_count(source.section_count), prof_count(source.prof_count), timeslot_count(source.timeslot_count) {
    valid = source.valid;
    sectionCredit = source.sectionCredit;
    sections = new Gene*[section_count];
    sectionTabooList = new bool *[section_count];

    fitness = source.fitness;
    for (int i = 0; i < section_count; i++) {
        sections[i] = new Gene(source.getProf(i), source.getTime(i));
        sectionTabooList[i] = new bool[timeslot_count];
        for (int j = 0; j < timeslot_count; ++j) {
            sectionTabooList[i][j] = source.sectionTabooList[i][j];
        }
    }

    professorCredits = new double[source.prof_count];
    professorCreditsInitial = source.professorCreditsInitial;
    for (int i = 0; i < source.prof_count; i++) {
        professorCredits[i] = source.professorCredits[i];
    }
}

/// <summary>
/// Updates the taboo list.
/// </summary>
/// <param name="incompatibleSectionsMatrix">The incompatible sections matrix.</param>
void Chromosome::updateTabooList(bool ** incompatibleSectionsMatrix) {
    for (int i = 0; i < section_count; ++i) {
        delete[] sectionTabooList[i];
        sectionTabooList[i] = new bool[timeslot_count]();
    }

    for (int first = 0; first < section_count - 1; ++first) {
        for (int second = first + 1; second < section_count; ++second) {
            if (incompatibleSectionsMatrix[first][second] || getProf(first) == getProf(second)) {
                sectionTabooList[first][getTime(second)] = true;
                sectionTabooList[second][getTime(first)] = true;
            }
        }
    }
}

/// <summary>
/// Gets the course load for the requested professor.
/// </summary>
/// <param name="profID">The prof identifier.</param>
/// <returns><c>Double</c> value of the requested professor's course load.</returns>
double Chromosome::getCourseLoad(int profID) {
    return professorCredits[profID];
}

/// <summary>
/// Gets the professor loads.
/// </summary>
/// <returns><c>String</c></returns>
string Chromosome::getProfessorLoads() {
    std::ostringstream ss;
    for (int i = 0; i < prof_count; i++) {
        ss << "(" << i << "," << fixed << setprecision(2) << professorCredits[i] << ")";
    }
    return ss.str();
}

/// <summary>
/// Finalizes an instance of the <see cref="Chromosome"/> class.
/// </summary>
Chromosome::~Chromosome() {
    if (sectionTabooList) {
        for (int i = 0; i < section_count; ++i) {
            delete[] sectionTabooList[i];
        }
        delete[] sectionTabooList;
    }
    if (sections) {
        for (int i = 0; i < section_count; ++i) {
            delete sections[i];
        }
        delete[] sections;
    }
    if (professorCredits)
        delete[] professorCredits;
}

/// <summary>
/// Determines whether this instance is valid.
/// </summary>
/// <returns><c>True</c> is the chromosome is valid, <c>false</c> otherwise.</returns>
bool Chromosome::isValid() {
    return valid;
}

/// <summary>
/// Determines whether the specified incompatible sections matrix is valid.
/// </summary>
/// <param name="incompatibleSectionsMatrix">The incompatible sections matrix.</param>
/// <param name="timeslotConflict">The timeslot conflict.</param>
/// <param name="verbose">if set to <c>true</c>, print all invalidities.</param>
/// <returns><c>True</c> is the chromosome is valid, <c>false</c> otherwise.</returns>
bool Chromosome::isValid(bool ** incompatibleSectionsMatrix, bool ** timeslotConflict, bool verbose) {
    validate(incompatibleSectionsMatrix, timeslotConflict);
    return valid;
}

/// <summary>
/// Assist sort function by sorting by professor credit.
/// </summary>
/// <param name="i"></param>
/// <param name="j"></param>
/// <returns>result of comparison <code>professorCredits[i] &lt; professorCredits[j] </code>.</returns>
bool Chromosome::sortByProfessorCredit(int i, int j) {
    return professorCredits[i] < professorCredits[j];
}

/// <summary>
/// Check if the current chromosome is equal to the other chromosome.
/// </summary>
/// <param name="other">The chromosome to compare with.</param>
/// <returns><c>True</c> if the two chromosomes are equal, <c>false</c> otherwise.</returns>
bool Chromosome::equals(Chromosome * other) {
    if (fitness != other->fitness)
        return false;
    for (int i = 0; i < section_count; ++i) {
        if (getProf(i) != other->getProf(i) || getTime(i) != other->getTime(i))
            return false;
    }
    return true;
}

/// <summary>
/// Sets the time for the given section
/// </summary>
/// <param name="sectionID">The section identifier.</param>
/// <param name="newTime">The new time.</param>
void Chromosome::setTime(int sectionID, int newTime) {
    if (sections[sectionID]) {
        sections[sectionID]->setTimeID(newTime);
    } else {
        sections[sectionID] = new Gene(-1, newTime);
    }
}

/// <summary>
/// Sets the professor for the given section
/// </summary>
/// <param name="sectionID">The section identifier.</param>
/// <param name="newProf">The new prof.</param>
void Chromosome::setProf(int sectionID, int newProf) {
    if (sections[sectionID]) {
    	if(getProf(sectionID) > -1){
        professorCredits[getProf(sectionID)] += sectionCredit[sectionID];
      }
      sections[sectionID]->setProfID(newProf);
    } else {
      sections[sectionID] = new Gene(newProf, -1);
    }
    professorCredits[newProf] -= sectionCredit[sectionID];
}

/// <summary>
/// Sets the gene for the given section.
/// </summary>
/// <param name="sectionID">The section identifier.</param>
/// <param name="newGene">The new gene.</param>
void Chromosome::setGene(int sectionID, Gene newGene) {
    if (sections[sectionID]) {
        professorCredits[getProf(sectionID)] += sectionCredit[sectionID];
        delete sections[sectionID];
    }
    sections[sectionID] = new Gene(newGene);
    professorCredits[getProf(sectionID)] -= sectionCredit[sectionID];
}

/// <summary>
/// Gets the time for the given section.
/// </summary>
/// <param name="sectionID">The section identifier.</param>
/// <returns><c>Integer</c> value of the timeslot ID.</returns>
const int Chromosome::getTime(int sectionID) {
    return sections[sectionID]->getTimeID();
}

/// <summary>
/// Gets the professor for the given section.
/// </summary>
/// <param name="sectionID">The section identifier.</param>
/// <returns><c>Integer</c> value of the professor ID.</returns>
const int Chromosome::getProf(int sectionID) {
    return sections[sectionID]->getProfID();
}

/// <summary>
/// Gets the gene for the given section.
/// </summary>
/// <param name="sectionID">The section identifier.</param>
/// <returns>A pointer to the <see cref="Gene"/> object for the section.</returns>
Gene* Chromosome::getGene(int sectionID) {
    return sections[sectionID];
}

/// <summary>
/// Gets the fitness.
/// </summary>
/// <returns><c>Integer</c> value of the fitness.</returns>
int Chromosome::getFitness() {
    return fitness;
}

/// <summary>
/// Validate the professor load for the given professor
/// </summary>
/// <param name="p">The professor ID to validate</param>
/// <returns><c>True</c> if the professor credits are within the valid range, <c>false</c> otherwise.</returns>
bool Chromosome::validProfessorLoad(int p) {
    return (professorCredits[p] >= DELTA_MAX && professorCredits[p] <= DELTA_MIN);
}

/// <summary>
/// Check if the professor load will be valid after subtracting the credit value;
/// </summary>
/// <param name="p">The professor to validate.</param>
/// <param name="credit">The credit.</param>
/// <returns><c>True</c> if the credits will not invalidate the professor's load, <c>false</c> otherwise.</returns>
bool Chromosome::validProfessorLoadChange(int p, double credit) {
    return ((professorCredits[p] - credit) >= DELTA_MAX || (professorCredits[p] - credit) <= DELTA_MIN);
}

string Chromosome::print() {
    string rtnVal;
    for (int i = 0; i < section_count; i++) {
        if (DEBUG_OUTPUT)
            rtnVal += "s";
        rtnVal += to_string(i) + ", " + print(i) + "\n";
    }

    rtnVal += "----------------------------\n";

    return rtnVal;
}

/// <summary>
/// Prints the specified section identifier.
/// </summary>
/// <param name="sectionID">The section identifier.</param>
/// <returns></returns>
string Chromosome::print(int sectionID) {
    if (DEBUG_OUTPUT)
        return "p" + to_string(getProf(sectionID)) + ", t" + to_string(getTime(sectionID));
    else
        return to_string(getProf(sectionID)) + ", " + to_string(getTime(sectionID));
}

/// <summary>
/// Evolves the current chromosome.
/// </summary>
/// <param name="sortedSectionList">The sorted section list.</param>
/// <param name="sectionProf">The section prof.</param>
/// <param name="sectionTimeslot">The section timeslot.</param>
/// <param name="h">The helper object.</param>
/// <param name="mutation_probability">The mutation probability.</param>
/// <param name="incompatibleSectionsMatrix">The incompatible sections matrix.</param>
/// <param name="timeslotConflict">The timeslot conflict.</param>
/// <param name="credit_count">The credit count.</param>
/// <param name="profSection">The prof section.</param>
/// <param name="associatedProfessors">The associated professors.</param>
/// <param name="sectionPref">The section preference.</param>
/// <param name="profPref">The prof preference.</param>
/// <param name="timeslotDaytime">The timeslot daytime.</param>
/// <param name="timeslotConsecutive">The timeslot consecutive.</param>
/// <param name="timeslotSpread">The timeslot spread.</param>
void Chromosome::evolve(int * const sortedSectionList, int ** const sectionProf, int ** const sectionTimeslot, Helper * const h, int const mutation_probability,
        bool ** const incompatibleSectionsMatrix, bool ** const timeslotConflict, int credit_count, int ** const profSection, int *** const associatedProfessors, int ** const sectionPref,
        int ** const profPref, bool ** const timeslotDaytime, bool ** const timeslotConsecutive, bool ** const timeslotSpread) {
    updateTabooList(incompatibleSectionsMatrix);
    mutate(incompatibleSectionsMatrix, sortedSectionList, sectionProf, sectionTimeslot, h, mutation_probability);
    repair(sortedSectionList, incompatibleSectionsMatrix, timeslotConflict, sectionTimeslot, sectionProf, profSection, associatedProfessors);
    updateFitness(incompatibleSectionsMatrix, sectionPref, profPref, timeslotDaytime, timeslotConflict, timeslotConsecutive, timeslotSpread);
    if (isValid())
        optimize(sectionProf, sectionTimeslot, h, profSection, sectionPref, profPref, incompatibleSectionsMatrix, timeslotDaytime, timeslotConflict, timeslotConsecutive, timeslotSpread);
}

string Chromosome::evolveVerbose(int * const sortedSectionList, int ** const sectionProf, int ** const sectionTimeslot, Helper * const h, int const mutation_probability,
        bool ** const incompatibleSectionsMatrix, bool ** const timeslotConflict, int credit_count, int ** const profSection, int *** const associatedProfessors, int ** const sectionPref,
        int ** const profPref, bool ** const timeslotDaytime, bool ** const timeslotConsecutive, bool ** const timeslotSpread) {
    stringstream ss;
    ss << "Evolving Verbose" << endl;
    updateTabooList(incompatibleSectionsMatrix);
    ss << "Pre-mutation" << endl << print();
    mutate(incompatibleSectionsMatrix, sortedSectionList, sectionProf, sectionTimeslot, h, mutation_probability);
    ss << "Post-mutation" << endl << print();
    ss << "Pre-Repair: " << endl << validateVerbose(incompatibleSectionsMatrix, timeslotConflict);
    debug.clear();
    repair(sortedSectionList, incompatibleSectionsMatrix, timeslotConflict, sectionTimeslot, sectionProf, profSection, associatedProfessors);
    ss << debug.str();
    ss << "Post-Repair: " << endl << validateVerbose(incompatibleSectionsMatrix, timeslotConflict);
    updateFitness(incompatibleSectionsMatrix, sectionPref, profPref, timeslotDaytime, timeslotConflict, timeslotConsecutive, timeslotSpread);

    if (isValid())
        optimize(sectionProf, sectionTimeslot, h, profSection, sectionPref, profPref, incompatibleSectionsMatrix, timeslotDaytime, timeslotConflict, timeslotConsecutive, timeslotSpread);
    return ss.str();
}

/// <summary>
/// Mutates the current chromosome.
/// </summary>
/// <param name="incompatibleSectionsMatrix">The incompatible sections matrix.</param>
/// <param name="sortedSectionList">The sorted section list.</param>
/// <param name="sectionProf">The section prof.</param>
/// <param name="sectionTimeslot">The section timeslot.</param>
/// <param name="h">The helper object.</param>
/// <param name="mutation_probability">The mutation probability.</param>
void Chromosome::mutate(bool ** incompatibleSectionsMatrix, int * sortedSectionList, int ** sectionProf, int ** sectionTimeslot, Helper * h, int mutation_probability) {

    if (DEBUG_MUTATION)
        cout << "Mutating Individual... " << endl;

    for (int idx = 0; idx < section_count; idx++) {
        int g = sortedSectionList[idx];
        int timeID = -1, compTimes = -1, profID = -1, compProfs = -1;

        compProfs = sectionProf[g][0];
        profID = getProf(g);

        compTimes = sectionTimeslot[g][0];
        timeID = getTime(g);

        int mutationTime = h->randNum(1, 100);
        if (mutationTime <= mutation_probability) {
            if (compTimes != 1) {
                timeID = h->randNum(1, compTimes);
                if (sectionTabooList[g][sectionTimeslot[g][timeID]])
                    for (timeID = 1; timeID <= compTimes && sectionTabooList[g][sectionTimeslot[g][timeID]]; ++timeID)
                        ;
            } else
                timeID = 1;
            setTime(g, sectionTimeslot[g][timeID]);
            updateTabooList(incompatibleSectionsMatrix);
        }

        int mutationProf = h->randNum(1, 100);
        if (mutationProf <= mutation_probability) {
            if (compProfs != 1) {
                profID = h->randNum(1, compProfs);
                if (profID != getProf(g))
                    setProf(g, sectionProf[g][profID]);
            } else
                profID = 1;
        }
    }

    if (DEBUG_MUTATION)
        cout << "Finished Mutating." << endl;
}

void Chromosome::fillEmpty(bool ** incompatibleSectionsMatrix, int * sortedSectionList, int ** sectionProf, int ** sectionTimeslot, Helper * h){
		if (DEBUG_MUTATION)
        cout << "Mutating Individual... " << endl;

    for (int g = 0; g < section_count; g++) {
        //If both professor and time values are assigned, continue to the next gene.
        if(getProf(g) > -1 && getTime(g) > -1){
        	continue;
        }
        
        //Otherwise, we need to fill data.
        
        int timeID = -1, compTimes = -1, profID = -1, compProfs = -1;

        compProfs = sectionProf[g][0];
        profID = getProf(g);

        compTimes = sectionTimeslot[g][0];
        timeID = getTime(g);

        if (compTimes != 1) {
            timeID = h->randNum(1, compTimes);
        } else
            timeID = 1;
        setTime(g, sectionTimeslot[g][timeID]);

        if (compProfs != 1) {
            profID = h->randNum(1, compProfs);
        } else
            profID = 1;
        setProf(g, sectionProf[g][profID]);
    }
    
    //updateTabooList(incompatibleSectionsMatrix);

    if (DEBUG_MUTATION)
        cout << "Finished Mutating." << endl;
}

/// <summary>
/// Shifts the section to overloaded.
/// </summary>
/// <param name="target">The target.</param>
/// <param name="parentID">The parent identifier.</param>
/// <param name="profID">The prof identifier.</param>
/// <param name="sectionProf">The section prof.</param>
/// <param name="profSection">The prof section.</param>
/// <param name="associatedProfessors">The associated professors.</param>
/// <returns>A string containing the professor to shift and the course to shift to the professor.</returns>
string Chromosome::shiftSectionToOverloaded(int target, int parentID, int profID, int ** sectionProf, int ** profSection, int *** associatedProfessors) {
    if (DEBUG_BALANCEPROFLOAD)
        debug << "Shift Section with parent: " << parentID << " and prof " << profID << endl;
    visitedProfessors[profID] = true;
    if (professorCredits[profID] > 0) {
        CURRENTPROFUNDERLOADED: int profIDIndex = 1;
        for (; profIDIndex <= associatedProfessors[parentID][0][0]; ++profIDIndex) {
            if (profID == associatedProfessors[parentID][profIDIndex][0])
                break;
        }
        for (int sTeach = 2; sTeach <= associatedProfessors[parentID][profIDIndex][1] + 1; ++sTeach) {
            if (getProf(associatedProfessors[parentID][profIDIndex][sTeach]) == parentID) {
                return to_string(associatedProfessors[parentID][profIDIndex][sTeach]) + "," + to_string(associatedProfessors[parentID][profIDIndex][0]);
            }
        }
        if (DEBUG_BALANCEPROFLOAD)
            debug << "Unable to find a shared section between " << parentID << " and prof " << profID << endl;
        if (professorCredits[profID] > 0)
            visitedProfessors[profID] = false;
        return "";
    }

    for (int associateIndex = 1; associateIndex <= associatedProfessors[profID][0][0]; ++associateIndex) {
        if (professorCredits[profID] > 0)
            goto CURRENTPROFUNDERLOADED;
        if (visitedProfessors[associatedProfessors[profID][associateIndex][0]] || professorCredits[associatedProfessors[profID][associateIndex][0]] < 0
                || associatedProfessors[profID][associateIndex][0] == target)
            continue;
        bool good = false;
        for (int x = 2; x <= associatedProfessors[profID][associateIndex][1] + 1 && !good; ++x) {
            if (getProf(associatedProfessors[profID][associateIndex][x]) == profID)
                good = true;
        }
        if (!good)
            continue;
        string sectionToTransfer = shiftSectionToOverloaded(target, profID, associatedProfessors[profID][associateIndex][0], sectionProf, profSection, associatedProfessors);
        if (!sectionToTransfer.empty()) {
            vector<string> data = Utility::Tokenize(sectionToTransfer, ',');
            int newProf = stoi(data.at(1));
            int section = stoi(data.at(0));
            setProf(section, newProf);
            if (professorCredits[newProf] > 0)
                visitedProfessors[newProf] = false;
            int profIDIndex = 1;
            for (; profIDIndex <= associatedProfessors[parentID][0][0]; ++profIDIndex) {
                if (profID == associatedProfessors[parentID][profIDIndex][0])
                    break;
            }
            for (int sTeach = 2; sTeach <= associatedProfessors[parentID][profIDIndex][1] + 1; ++sTeach) {
                if (getProf(associatedProfessors[parentID][profIDIndex][sTeach]) == parentID) {
                    return to_string(associatedProfessors[parentID][profIDIndex][sTeach]) + "," + to_string(associatedProfessors[parentID][profIDIndex][0]);
                }
            }
        }
    }
    return "";
}

/// <summary>
/// Shifts the section to underloaded.
/// </summary>
/// <param name="target">The target.</param>
/// <param name="parentID">The parent identifier.</param>
/// <param name="profID">The prof identifier.</param>
/// <param name="sectionProf">The section prof.</param>
/// <param name="profSection">The prof section.</param>
/// <param name="associatedProfessors">The associated professors.</param>
/// <returns></returns>
string Chromosome::shiftSectionToUnderloaded(int target, int parentID, int profID, int ** sectionProf, int ** profSection, int *** associatedProfessors) {
    if (DEBUG_BALANCEPROFLOAD)
        debug << "Shift Section with parent: " << parentID << " and prof " << profID << endl;
    visitedProfessors[profID] = true;
    if (professorCredits[profID] < 0) {
        CURRENTPROFOVERLOADED: int profIDIndex = 1;
        for (; profIDIndex <= associatedProfessors[parentID][0][0]; ++profIDIndex) {
            if (profID == associatedProfessors[parentID][profIDIndex][0])
                break;
        }
        for (int sTeach = 2; sTeach <= associatedProfessors[parentID][profIDIndex][1] + 1; ++sTeach) {
            if (getProf(associatedProfessors[parentID][profIDIndex][sTeach]) == profID) {
                return to_string(associatedProfessors[parentID][profIDIndex][sTeach]) + "," + to_string(associatedProfessors[parentID][profIDIndex][0]);
            }
        }
        if (DEBUG_BALANCEPROFLOAD)
            debug << "Unable to find a shared section between " << parentID << " and prof " << profID << endl;
        if (professorCredits[profID] < 0)
            visitedProfessors[profID] = false;
        return "";
    }

    for (int associateIndex = 1; associateIndex <= associatedProfessors[profID][0][0]; ++associateIndex) {
        if (professorCredits[profID] < 0)
            goto CURRENTPROFOVERLOADED;
        if (visitedProfessors[associatedProfessors[profID][associateIndex][0]] || professorCredits[associatedProfessors[profID][associateIndex][0]] > 0
                || associatedProfessors[profID][associateIndex][0] == target)
            continue;
        bool good = false;
        for (int x = 2; x <= associatedProfessors[profID][associateIndex][1] + 1 && !good; ++x) {
            if (getProf(associatedProfessors[profID][associateIndex][x]) == associatedProfessors[profID][associateIndex][0])
                good = true;
        }
        if (!good)
            continue;
        string sectionToTransfer = shiftSectionToUnderloaded(target, profID, associatedProfessors[profID][associateIndex][0], sectionProf, profSection, associatedProfessors);
        if (!sectionToTransfer.empty()) {
            vector<string> data = Utility::Tokenize(sectionToTransfer, ',');
            int newProf = stoi(data.at(1));
            int section = stoi(data.at(0));
            setProf(section, profID);
            if (professorCredits[newProf] < 0)
                visitedProfessors[newProf] = false;
            int profIDIndex = 1;
            for (; profIDIndex <= associatedProfessors[parentID][0][0]; ++profIDIndex) {
                if (profID == associatedProfessors[parentID][profIDIndex][0])
                    break;
            }
            for (int sTeach = 2; sTeach <= associatedProfessors[parentID][profIDIndex][1] + 1; ++sTeach) {
                if (getProf(associatedProfessors[parentID][profIDIndex][sTeach]) == profID) {
                    return to_string(associatedProfessors[parentID][profIDIndex][sTeach]) + "," + to_string(associatedProfessors[parentID][profIDIndex][0]);
                }
            }
        }
    }
    return "";
}

/// <summary>
/// Balances the prof load.
/// </summary>
/// <param name="sectionProf">The section prof.</param>
/// <param name="profSection">The prof section.</param>
/// <param name="associatedProfessors">The associated professors.</param>
void Chromosome::balanceProfLoad(int ** sectionProf, int ** profSection, int *** associatedProfessors) {
    int prevProfBalancedCount = 0;
    int currentProfBalancedCount = prof_count;
    int * profBackupList = new int[section_count];
    int tries = 0;
    do {
        debug << "Current balancing try: " << tries << endl << endl;
        for (int i = 0; i < section_count; ++i) {
            profBackupList[i] = getProf(i);
        }

        prevProfBalancedCount = currentProfBalancedCount;
        currentProfBalancedCount = 0;
        for (int profID = 0; profID < prof_count; ++profID) {
            if (professorCredits[profID] < DELTA_MAX) {
                currentProfBalancedCount++;
                visitedProfessors = new bool[prof_count]();
                visitedProfessors[profID] = true;
                if (DEBUG_BALANCEPROFLOAD)
                    debug << "Starting with " << profID;
                //Go through the associates to find a professor that can transfer classes.
                while (professorCredits[profID] < DELTA_MAX) {
                    int associateIndex = 1;
                    for (; associateIndex <= associatedProfessors[profID][0][0] && professorCredits[profID] < DELTA_MAX; ++associateIndex) {
                        if (DEBUG_BALANCEPROFLOAD)
                            debug << " Associate: " << associatedProfessors[profID][associateIndex][0] << endl;
                        bool good = false;
                        for (int x = 2; x <= associatedProfessors[profID][associateIndex][1] + 1; ++x) {
                            if (getProf(associatedProfessors[profID][associateIndex][x]) == profID) {
                                good = true;
                                break;
                            }
                        }
                        if (!good)
                            continue;
                        string sectionToShift = shiftSectionToOverloaded(profID, profID, associatedProfessors[profID][associateIndex][0], sectionProf, profSection, associatedProfessors);
                        if (sectionToShift.empty())
                            continue;
                        vector<string> data = Utility::Tokenize(sectionToShift, ',');
                        int newProf = stoi(data.at(1));
                        int section = stoi(data.at(0));
                        setProf(section, newProf);
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
                        debug << "Unable to balance " << profID << " :(" << endl;
                    else
                        debug << profID << " balanced. :)" << endl;
                }
            } else if (professorCredits[profID] > DELTA_MIN) {
                currentProfBalancedCount++;
                visitedProfessors = new bool[prof_count]();
                visitedProfessors[profID] = true;
                if (DEBUG_BALANCEPROFLOAD)
                    debug << "Starting with " << profID;

                //Go through the associates to find a professor that can transfer classes.
                while (professorCredits[profID] > DELTA_MIN) {
                    int associateIndex = 1;
                    for (; associateIndex <= associatedProfessors[profID][0][0] && professorCredits[profID] > DELTA_MIN; ++associateIndex) {
                        if (DEBUG_BALANCEPROFLOAD)
                            debug << " Associate: " << associatedProfessors[profID][associateIndex][0] << endl;
                        string sectionToShift = shiftSectionToUnderloaded(profID, profID, associatedProfessors[profID][associateIndex][0], sectionProf, profSection, associatedProfessors);
                        if (sectionToShift.empty())
                            continue;
                        vector<string> data = Utility::Tokenize(sectionToShift, ',');
                        int newProf = stoi(data.at(1));
                        int section = stoi(data.at(0));
                        setProf(section, profID);
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
                        debug << "Unable to balance " << profID << " :(" << endl;
                    else
                        debug << profID << " balanced. :)" << endl;
                }
            }
        }
    } while (prevProfBalancedCount > currentProfBalancedCount && currentProfBalancedCount != 0 && ++tries < REPAIR_MAX);
    if (currentProfBalancedCount > prevProfBalancedCount) {
        for (int i = 0; i < section_count; ++i) {
            setProf(i, profBackupList[i]);
        }
    }
    delete[] profBackupList;
}

/// <summary>
/// Check if the professors are balanced.
/// </summary>
/// <returns><c>True</c> if the professors are balanced, <c>false</c> otherwise.</returns>
bool Chromosome::professorsBalanced() {
    for (int i = 0; i < prof_count; ++i) {
        if (!(professorCredits[i] >= DELTA_MAX && professorCredits[i] <= DELTA_MIN))
            return false;
    }
    return true;
}

/// <summary>
/// Repairs the current chromosome.
/// </summary>
/// <param name="sortedSectionList">The sorted section list.</param>
/// <param name="incompatibleSectionsMatrix">The incompatible sections matrix.</param>
/// <param name="timeslotConflict">The timeslot conflict.</param>
/// <param name="sectionTimeslot">The section timeslot.</param>
/// <param name="sectionProf">The section prof.</param>
/// <param name="profSection">The prof section.</param>
/// <param name="associatedProfessors">The associated professors.</param>
void Chromosome::repair(int * sortedSectionList, bool ** incompatibleSectionsMatrix, bool ** timeslotConflict, int ** sectionTimeslot, int ** sectionProf, int ** profSection, int *** associatedProfessors) {
    int retry = 0;
    do {
        debug << "Repair try: " << retry << endl;
        balanceProfLoad(sectionProf, profSection, associatedProfessors);
        /*bool ** sectionTimeTabooList = new bool*[section_count];
        for (int i = 0; i < section_count; ++i) {
            sectionTimeTabooList[i] = new bool[timeslot_count]();
        }*/
        updateTabooList(incompatibleSectionsMatrix);
        for (int index = 0; index < section_count; ++index) {
            int currentSection = sortedSectionList[index];
            for (int otherSection = 0; otherSection < section_count; ++otherSection) {
                if (currentSection == otherSection)
                    continue;
                if (incompatibleSectionsMatrix[currentSection][otherSection] || getProf(currentSection) == getProf(otherSection)) {
                    if (timeslotConflict[getTime(currentSection)][getTime(otherSection)]) {
                        //sectionTimeTabooList[currentSection][getTime(currentSection)] = true;
                        int potentialTimeslotIndex = 1;
                        for (; potentialTimeslotIndex <= sectionTimeslot[currentSection][0] && /*sectionTimeTabooList[currentSection][sectionTimeslot[currentSection][potentialTimeslotIndex]]*/sectionTabooList[currentSection][sectionTimeslot[currentSection][potentialTimeslotIndex]]; ++potentialTimeslotIndex) ;
                        setTime(currentSection, sectionTimeslot[currentSection][potentialTimeslotIndex]);
                        updateTabooList(incompatibleSectionsMatrix);
                    }
                }
            }
        }
        /*for (int i = 0; i < section_count; ++i) {
            delete[] sectionTimeTabooList[i];
        }
        delete[] sectionTimeTabooList;*/
        validate(incompatibleSectionsMatrix, timeslotConflict);
        debug << "Validity: " << endl << validateVerbose(incompatibleSectionsMatrix, timeslotConflict);
    } while (!valid && ++retry < REPAIR_MAX);
}

/// <summary>
/// Optimizes the current chromosome.
/// </summary>
/// <param name="sectionProf">The section prof.</param>
/// <param name="sectionTimeslot">The section timeslot.</param>
/// <param name="h">The h.</param>
/// <param name="profSection">The prof section.</param>
/// <param name="sectionPref">The section preference.</param>
/// <param name="profPref">The prof preference.</param>
/// <param name="incompatibleSectionsMatrix">The incompatible sections matrix.</param>
/// <param name="timeslotDaytime">The timeslot daytime.</param>
/// <param name="timeslotConflict">The timeslot conflict.</param>
/// <param name="timeslotConsecutive">The timeslot consecutive.</param>
/// <param name="timeslotSpread">The timeslot spread.</param>
void Chromosome::optimize(int ** sectionProf, int ** sectionTimeslot, Helper * h, int ** profSection, int ** sectionPref, int ** profPref, bool ** incompatibleSectionsMatrix, bool ** timeslotDaytime,
        bool ** timeslotConflict, bool ** timeslotConsecutive, bool ** timeslotSpread) {
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
    for (; sectionIdx < section_count; ++sectionIdx) {
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
            for (int newSectionIdx = 0; newSectionIdx < section_count && !swapSuccess; ++newSectionIdx) {
                if (getProf(newSectionIdx) == profToSwapWith) {
                    //Can the currentProf teach this section
                    for (int canTeach = 1; canTeach <= sectionProf[newSectionIdx][0] && !swapSuccess; ++canTeach) {
                        if (sectionProf[newSectionIdx][canTeach] == currentProf) {
                            //Perform swap
                            setProf(sectionIdx, profToSwapWith);
                            setProf(newSectionIdx, currentProf);
                            //check valid
                            validate(incompatibleSectionsMatrix, timeslotConflict);
                            updateFitness(incompatibleSectionsMatrix, sectionPref, profPref, timeslotDaytime, timeslotConflict, timeslotConsecutive, timeslotSpread);
                            //Check if swap was valid and it is better fitness
                            if (!valid && fitness > startingFitness) {
                                //Undo the swap
                                setProf(sectionIdx, currentProf);
                                setProf(newSectionIdx, profToSwapWith);
                                continue;
                            } else {
                                swapSuccess = true;
                                lastIndexLeft = sectionIdx;
                                lastIndexRight = newSectionIdx;
                                lastProfLeft = currentProf;
                                lastProfRight = profToSwapWith;
                                startingFitness = fitness;
                            }
                        } //if(sectionProf[ newSectionIdx ][ canTeach ] == currentProf)
                    } //for(int canTeach = 1; canTeach <= sectionProf[ newSectionIdx ][ 0 ] && !swapSuccess; ++canTeach)
                } //if(getProf(newSectionIdx) == profToSwapWith)
            } // for(int newSectionIdx = 0; newSectionIdx < gene_length && !swapSuccess; ++newSectionIdx)
        } //if(sectionProf[ sectionIdx ][ 0 ] > 1)
    } //for(; sectionIdx < gene_length; ++sectionIdx)
}

/// <summary>
/// Validates the current chromosome
/// </summary>
/// <param name="incompatibleSectionsMatrix">The incompatible sections matrix.</param>
/// <param name="timeslotConflict">The timeslot conflict.</param>
/// <param name="verbose">if set to <c>true</c>, prints the invalidities.</param>
void Chromosome::validate(bool ** incompatibleSectionsMatrix, bool ** timeslotConflict) {
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
            valid = false;
            localValid = false;
        } else {
            if (DEBUG_VALIDATE) {
                cout << endl;
            }
        }
    }

    for (int left = 0; left < section_count && valid; ++left) {
        int leftProf = getProf(left);
        for (int right = left + 1; right < section_count && valid; ++right) {
            int rightProf = getProf(right);
            if (leftProf == rightProf && timeslotConflict[getTime(left)][getTime(right)]) {
                valid = false;
                localValid = false;
                break;
                //}
            }
        }
    }

    for (int left = 0; left < section_count && valid; ++left) {
        int leftTime = getTime(left);
        for (int right = left + 1; right < section_count && valid; ++right) {
            if (!incompatibleSectionsMatrix[left][right])
                continue;
            int rightTime = getTime(right);
            if (leftTime == rightTime) {
                valid = false;
                localValid = false;
                break;
            }
        }
    }
}

string Chromosome::validateVerbose(bool ** incompatibleSectionsMatrix, bool ** timeslotConflict) {
    valid = true;
    bool localValid = true;
    stringstream ss;
    /*
     First, I will try to ensure that a professor is not scheduled twice at the same timeID.
     Then, I will look to ensure that no incompatible classes are scheduled at the same timeID.
     */

    for (int pr = 0; pr < prof_count && valid; pr++) {
        if (!(getCourseLoad(pr) >= DELTA_MAX && getCourseLoad(pr) <= DELTA_MIN)) {
            ss << 'p' << pr << " UNBALANCED with " << getCourseLoad(pr) << endl;

            valid = false;
            localValid = false;
        } else {
            if (DEBUG_VALIDATE) {
                cout << endl;
            }
        }
    }

    for (int left = 0; left < section_count && valid; ++left) {
        int leftProf = getProf(left);
        for (int right = left + 1; right < section_count && valid; ++right) {
            int rightProf = getProf(right);
            if (leftProf == rightProf && timeslotConflict[getTime(left)][getTime(right)]) {
                ss << "Incompatible Prof: p" << left << " with p" << right << endl;
                valid = false;
                localValid = false;
                break;
            }
        }
    }

    for (int left = 0; left < section_count && valid; ++left) {
        int leftTime = getTime(left);
        for (int right = left + 1; right < section_count && valid; ++right) {
            if (!incompatibleSectionsMatrix[left][right])
                continue;
            int rightTime = getTime(right);
            if (leftTime == rightTime) {
                ss << "Incompatible Sections: s" << left << " with s" << right << endl;
                valid = false;
                localValid = false;
                break;
            }
        }
    }

    ss << "Validation result: " << localValid << endl;

    return ss.str();
}

/// <summary>
/// Updates the fitness.
/// </summary>
/// <param name="incompatibleSectionsMatrix">The incompatible sections matrix.</param>
/// <param name="sectionPref">The section preference.</param>
/// <param name="profPref">The prof preference.</param>
/// <param name="timeslotDaytime">The timeslot daytime.</param>
/// <param name="timeslotConflict">The timeslot conflict.</param>
/// <param name="timeslotConsecutive">The timeslot consecutive.</param>
/// <param name="timeslotSpread">The timeslot spread.</param>
void Chromosome::updateFitness(bool ** incompatibleSectionsMatrix, int ** sectionPref, int ** profPref, bool ** timeslotDaytime, bool ** timeslotConflict, bool ** timeslotConsecutive, bool ** timeslotSpread) {
    fitness = MAXFITNESS;
    //bool * profConflict = new bool[section_count];
    int numConflicts = 0;
    vector<int> * professorTimeSlotsAssigned = new vector<int> [prof_count];
    //Check for hard constraint violation for professor
    for (int left = 0; left < section_count; left++) {
        if (left < 0)
            cout << "Inside loop: " << section_count;
        int leftProf = getProf(left); //sections[left]->getProfID();
        professorTimeSlotsAssigned[leftProf].push_back(getTime(left)); //sections[left]->getTimeID());
        for (int right = left + 1; right < section_count; right++) {
            int rightProf = getProf(right); //sections[right]->getProfID();
            if (leftProf == rightProf) {
                int leftTime = getTime(left), //sections[left]->getTimeID(),
                rightTime = getTime(right); //sections[right]->getTimeID();
                if (leftTime == rightTime || timeslotConflict[leftTime][rightTime]) { // timeSlots[ leftTime ]->isOverlap(timeSlots[ rightTime ])) {
                    if (DEBUG_FITNESS)
                        cout << "Incompatible Prof Time: " << left << " with " << right << endl;
                    //Create penalty based on Max_Fitness divided by professor count.
                    fitness -= PENALTY_PROFESSOR_TIME_CONFLICT;
                    //profConflict[left] = true;
                } /*else
                    profConflict[left] = false;*/
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
            } else {
                fitness -= PENALTY_PROFESSOR_LOAD_MAJOR;
            }
        }
    }

    //Check for hard constraint violation for timeID conflict
    for (int left = 0; left < section_count; left++) {
        if (left < 0)
            cout << "Inside loop2: " << section_count;
        int leftTime = getTime(left); //sections[left]->getTimeID();
        for (int right = left + 1; right < section_count; ++right) {
            if (incompatibleSectionsMatrix[left][right] && getTime(right) == leftTime) {
                if (DEBUG_FITNESS)
                    cout << "Incompatible Time: " << left << " with " << right << endl;
                fitness -= PENALTY_SECTION_TIME_CONFLICT;
            }
        }
    }

    //Check for soft constraint for prof timeID preference.
    for (int secID = 0; secID < section_count; secID++) {
        int timeAlloted = timeslotDaytime[getTime(secID)][0] ? 0 : (timeslotDaytime[getTime(secID)][1] ? 1 : 2);

        if (DEBUG_FITNESS) {
            ; //cout << "Allotment: " << profPref[genes[secID].getProfID( )][timeAlloted] << " Penalty: " << MAX_FITNESS << "/" << pow(prof_count + gene_length, 2.0) << " = " << MAX_FITNESS / pow((prof_count + gene_length), 2.0) << endl;
        }
        fitness -= profPref[getProf(secID)][timeAlloted] * PENALTY_PROFESSOR_PREFERENCE;

    }

    //Check for soft constraint for section timeID preference.
    for (int secID = 0; secID < section_count; secID++) {
        int timeAlloted = timeslotDaytime[getTime(secID)][0] ? 0 : (timeslotDaytime[getTime(secID)][1] ? 1 : 2);
        fitness -= sectionPref[secID][timeAlloted] * PENALTY_SECTION_PREFERENCE;
    }

    //Check for soft constraint for more than 2 consecutive classes or spread out classes.
    for (int profIndex = 0; profIndex < prof_count; ++profIndex) {
        for (int first = 0; first < (signed) professorTimeSlotsAssigned[profIndex].size(); ++first) {
            for (int second = first + 1; second < (signed) professorTimeSlotsAssigned[profIndex].size(); ++second) {
                if (timeslotConsecutive[first][second]) { // timeSlots[ first ]->isConsecutive(timeSlots[ second ])) {
                    //Two consecutive timeslots, look for a third.
                    for (int third = second + 1; third < (signed) professorTimeSlotsAssigned[profIndex].size(); ++third) {
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

/// <summary>
/// Updates the professor load.
/// </summary>
/// <param name="sectionCredit">The section credit.</param>
void Chromosome::updateProfLoad() {
    //Reset all professor credit values
    for (int prof = 0; prof < prof_count; ++prof) {
        professorCredits[prof] = professorCreditsInitial[prof];
    }

    //Iterate through all sections and update the associated prof credit
    for (int section = 0; section < section_count; ++section) {
        professorCredits[getProf(section)] -= sectionCredit[section];
    }
}

/// <summary>
/// Prints the prof table.
/// </summary>
/// <param name="sectionCredit">The section credit.</param>
/// <returns></returns>
string Chromosome::printProfTable() {
    ostringstream ss;
    string * profs = new string[prof_count];

    string * profsCourseCredit = new string[prof_count];
    double * profSum = new double[prof_count]();

    for (int i = 0; i < prof_count; ++i) {
        if (DEBUG_OUTPUT)
            profs[i] = (i < 10 ? " p" : "p");
        else
            profs[i] = (i < 10 ? " " : "");
        profs[i] += to_string(i) + " [" + (professorCredits[i] < 0 ? "" : " ") + Utility::FormatDouble(professorCredits[i], 2) + "," + Utility::FormatDouble(professorCreditsInitial[i], 2) + "]: ";
        if (professorCredits[i] < DELTA_MAX || professorCredits[i] > DELTA_MIN)
            profs[i] = "# " + profs[i];
        else
            profs[i] = "  " + profs[i];
    }
    for (int i = 0; i < section_count; ++i) {
        profs[getProf(i)] += "s" + to_string(i) + ", ";
        if (DEBUG_PROF_LOAD) {
            profsCourseCredit[getProf(i)] += to_string(sectionCredit[i]) + " + ";
            profSum[getProf(i)] += sectionCredit[i];
        }
    }
    for (int i = 0; i < prof_count; ++i) {
        ss << profs[i];
        if (DEBUG_PROF_LOAD)
            ss << " & " << profsCourseCredit[i] << " = " << profSum[i] << "(" << (professorCreditsInitial[i] - profSum[i]) << ")" << endl;
        else
            ss << endl;
    }
    delete[] profs;
    delete[] profsCourseCredit;
    delete[] profSum;
    return ss.str();
}

/// <summary>
/// Counts the number of points of difference between current chromosome and parent chromosome.
/// </summary>
/// <param name="parent">The parent.</param>
/// <returns><c>Integer</c> count of the number of difference.</returns>
int Chromosome::DifferenceCount(Chromosome * const parent) {
    int diffs = 0;
    for (int i = 0; i < section_count; ++i) {
        if (getProf(i) != parent->getProf(i)) {
            diffs++;
        }
        if (getTime(i) != parent->getTime(i)) {
            diffs++;
        }
    }

    return diffs;
}
