
#include "Population.h"

int main( ) {
    Population p("inputFiles.dat", 10, 5000, 100, 0.3);
    cout << "Begin Evolution" << endl;
    p.Evolve( );
    cout << endl << "End Evolution" << endl;
    cout << "*********************************************************************" << endl;
    cout << "Final Population:" << endl;
    cout << p.PrintTableFormat( );
    cout << "*********************************************************************" << endl;
    //system("PAUSE");
}