#include "Gene.h"


Gene::Gene( ) {
    profID = -1;
    timeID = -1;
}


Gene::~Gene( ) {
}

void Gene::setProfID(int prof) {
    if (prof > -1)
        profID = prof;
}

void Gene::setTimeID(int time) {
    if (time > -1)
        timeID = time;
}

int Gene::getProfID( ) {
    return profID;
}

int Gene::getTimeID( ) {
    return timeID;
}

ostream& operator<<(ostream &out, Gene &g) {
    out << g.timeID << "," << g.profID;
    return out;
}