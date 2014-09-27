#include "Gene.h"


Gene::Gene( ) {
    profID = -1;
    timeID = -1;
}


Gene::~Gene( ) {
}

void Gene::setProfID(int prof) {
    profID = prof;
}

void Gene::setTimeID(int time) {
    timeID = time;
}

int Gene::getProfID( ) {
    return profID;
}

int Gene::getTimeID( ) {
    return timeID;
}
