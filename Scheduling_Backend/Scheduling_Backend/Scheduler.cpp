
#include "Population.h"

int main( ) {
    Population p("inputFiles.dat", 50, 5000, 200, 0.3);
    cout << "Begin Evolution" << endl;
    p.Evolve( );
    cout << endl << "End Evolution" << endl;
    cout << "*********************************************************************" << endl;
    cout << "Final Population:" << endl;
    cout << p;
    cout << endl << endl << "All Done." << endl;
    //system("PAUSE");
}