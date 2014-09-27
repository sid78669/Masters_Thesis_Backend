#include "TimeGene.h"


TimeGene::TimeGene( ) {
    profID = -1;
    sectionID = -1;
}


TimeGene::~TimeGene( ) {
}

void TimeGene::setProfID(int prof) {
    profID = prof;
}

void TimeGene::setSectionID(int section) {
    sectionID = section;
}

int TimeGene::getProfID( ) {
    return profID;
}

int TimeGene::getSectionID( ) {
    return sectionID;
}
