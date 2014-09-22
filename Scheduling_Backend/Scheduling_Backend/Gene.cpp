#include "Gene.h"


Gene::Gene( ) {
    profID = 0;
    timeID = 0;
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
