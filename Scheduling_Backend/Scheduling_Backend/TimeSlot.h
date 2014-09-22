#ifndef TIMESLOT_H
#define TIMESLOT_H

#include <iostream>
#include <string>
using namespace std;
class TimeSlot {
public:
    struct Period {
        int startTime;
        int endTime;
    };

    
    TimeSlot(double);
    TimeSlot(TimeSlot*&);
    ~TimeSlot( );
        
    void setPeriod(int, string);
    void setCredits(double);
    
    double getCredits( );
    bool isOverlap(TimeSlot);

    string print( );
    friend ostream& operator<<(ostream&, const TimeSlot&);
private:
    Period days[6];
    double credits;
    string dayTitle[6];// = { "M", "T", "W", "R", "F", "S" };
};

#endif 