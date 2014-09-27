#ifndef TIMEGENE_H
#define TIMEGENE_H

class TimeGene {
public:
    TimeGene( );
    ~TimeGene( );
    void setProfID(int);
    void setSectionID(int);

    int getProfID( );
    int getSectionID( );

private:
    int sectionID, profID;
};

#endif