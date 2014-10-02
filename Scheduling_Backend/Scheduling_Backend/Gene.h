#ifndef GENE_H
#define GENE_H

#include <string>
#include <iostream>
using namespace std;

class Gene {
public:
    Gene( );
    ~Gene( );
    void setProfID(int);
    void setTimeID(int);

    int getProfID( );
    int getTimeID( );

private:
    int profID;
    int timeID;

    friend ostream& operator<<(ostream &out, Gene &g);
};

#endif