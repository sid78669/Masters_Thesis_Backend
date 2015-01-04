/*
Author: Siddharth Dahiya
Package: Course and Professor Scheduling (Backend)
File: TimeSlot.h
Contact: syd5144@gmail.com

Copyright (c) 2015 Siddharth Dahiya

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/



#ifndef TIMESLOT_H
#define TIMESLOT_H
#define TIME_DELTA 15

#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <time.h>
using namespace std;
class TimeSlot {
public:
    struct Period {
        int startTime;
        int endTime;
    };


    TimeSlot(double);
    TimeSlot(const TimeSlot*&);
    TimeSlot(const TimeSlot &);
    virtual ~TimeSlot( );

    void setPeriod(int, string);
    void setCredits(double);

    double getCredits( );
    bool isOverlap(TimeSlot *);
    bool isMorning( );
    bool isAfternoon( );
    bool isEvening( );
    bool isConsecutive(TimeSlot *);
    bool isSpreadOut(TimeSlot *);
    int getStartQuarter(int day);
    int getEndQuarter(int day);

    string print(bool creditPrint = false);

    friend ostream& operator<<(ostream&, const TimeSlot&);
private:
    Period days[6];
    double credits;
    bool morning;
    bool afternoon;
    bool timeUnset;
    string dayTitle[6];// = { "M", "T", "W", "R", "F", "S" };
};

#endif 
