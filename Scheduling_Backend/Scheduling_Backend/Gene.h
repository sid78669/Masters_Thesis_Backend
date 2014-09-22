#ifndef GENE_H
#define GENE_H

#include <string>
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
};

#endif