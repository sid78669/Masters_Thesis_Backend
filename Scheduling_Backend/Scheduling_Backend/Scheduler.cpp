
#include "Population.h"

int main( ) {
    Population p("inputFiles.dat", 10, 500, 100, 0.3);
    cout << "Begin Evolution" << endl;
    p.Evolve( );
    cout << "End Evolution" << endl;
    cin;
}