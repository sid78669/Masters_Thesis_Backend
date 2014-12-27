
#include "Population.h"
#include "getCPUTime.c"

int main() {
    double startTime, endTime;
    startTime = getCPUTime();
    Population p("new_input.dat");
    //cout << "Begin Evolution" << endl;
    p.Evolve();
    cout << endl << "End Evolution" << endl;
    cout << "*********************************************************************" << endl;
    cout << "Final Population:" << endl;
    cout << p;
    cout << endl << endl << "All Done." << endl;
    endTime = getCPUTime();
    fprintf(stdout, "CPU time used = %lf\n", (endTime - startTime));
}