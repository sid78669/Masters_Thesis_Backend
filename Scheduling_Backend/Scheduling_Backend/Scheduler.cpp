
#include <vld.h> 
#include "Population.h"

int main( ) {
    Population p("new_input.dat");
    cout << "Begin Evolution" << endl;
    p.Evolve( );
    cout << endl << "End Evolution" << endl;
    cout << "*********************************************************************" << endl;
    cout << "Final Population:" << endl;
    cout << p;
    cout << endl << endl << "All Done." << endl;
    //system("PAUSE");
}