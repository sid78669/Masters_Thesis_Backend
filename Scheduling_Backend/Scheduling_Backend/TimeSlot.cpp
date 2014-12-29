/*
Author: Siddharth Dahiya
Package: Course and Professor Scheduling (Backend)
File: TimeSlot.cpp
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


#include "TimeSlot.h"

TimeSlot::TimeSlot(double creds) {
    credits = creds;
    dayTitle[0] = "M";
    dayTitle[1] = "T";
    dayTitle[2] = "W";
    dayTitle[3] = "R";
    dayTitle[4] = "F";
    dayTitle[5] = "S";
    timeUnset = false;
}

TimeSlot::TimeSlot(const TimeSlot * &other) {
    dayTitle[0] = "M";
    dayTitle[1] = "T";
    dayTitle[2] = "W";
    dayTitle[3] = "R";
    dayTitle[4] = "F";
    dayTitle[5] = "S";
    morning = other->morning;
    afternoon = other->afternoon;
    credits = other->credits;
    timeUnset = other->timeUnset;
    for (int i = 0; i < 6; i++)
        days[i] = other->days[i];
}

TimeSlot::TimeSlot(const TimeSlot &other) {
    dayTitle[0] = "M";
    dayTitle[1] = "T";
    dayTitle[2] = "W";
    dayTitle[3] = "R";
    dayTitle[4] = "F";
    dayTitle[5] = "S";
    morning = other.morning;
    afternoon = other.afternoon;
    credits = other.credits;
    timeUnset = other.timeUnset;
    for (int i = 0; i < 6; i++)
        days[i] = other.days[i];
}

TimeSlot::~TimeSlot( ) {
}

void TimeSlot::setPeriod(int day, string times) {
    int start = -1, end = -1;
    start = stoi(times.substr(times.find("(") + 1, times.find(":") - 1));
    end = stoi(times.substr(times.find(":") + 1, times.find(")") - 1));
    days[day].startTime = start;
    days[day].endTime = end;
    if (!timeUnset) {
        if (end > 0 && end < 1200) {
            morning = true;
            afternoon = false;
            timeUnset = true;
        } else if (start > 1200 && end < 1800) {
            morning = false;
            afternoon = true;
            timeUnset = true;
        } else {
            morning = false;
            afternoon = false;
            timeUnset = true;
        }
    }
}

void TimeSlot::setCredits(double newVal) {
    credits = newVal;
}

double TimeSlot::getCredits( ) {
    return credits;
}

bool TimeSlot::isConsecutive(TimeSlot other ) {    
    for(int i = 0; i < 6; i++) {
        struct tm currentDayStart, currentDayEnd, correspondingDayStart, correspondingDayEnd;
        currentDayStart.tm_hour = days[ i ].startTime / 100;
        currentDayEnd.tm_hour = days[ i ].endTime / 100;
        currentDayStart.tm_min = days[ i ].startTime % 100;
        currentDayEnd.tm_min = days[ i ].endTime % 100;
        correspondingDayStart.tm_hour = other.days[ i ].startTime / 100;
        correspondingDayEnd.tm_hour = other.days[ i ].endTime / 100;
        correspondingDayStart.tm_min = other.days[ i ].startTime % 100;
        correspondingDayEnd.tm_min = other.days[ i ].endTime % 100;

        double secondsSE = difftime(mktime(&currentDayStart), mktime(&correspondingDayEnd));
        double secondsES = difftime(mktime(&currentDayEnd), mktime(&correspondingDayStart));

        //If the difference in time is less than 15 minutes, they are consecutive.
        if(secondsSE <= 60 * 16 || secondsES <= 60*16) {
            return true;
        }
    }

    return false;
}

bool TimeSlot::isOverlap(TimeSlot other) {
    for (int i = 0; i < 6; i++) {
        if (days[i].startTime >= other.days[i].startTime && days[i].startTime < other.days[i].endTime) {
            return true;
        } else if (days[i].startTime <= other.days[i].startTime && days[i].endTime >= other.days[i].startTime) {
            return true;
        } else if (days[i].endTime >= other.days[i].startTime && days[i].endTime <= other.days[i].endTime) {
            return true;
        } else if (other.days[i].endTime >= days[i].startTime && other.days[i].endTime <= days[i].startTime) {
            return true;
        }
    }
    return false;
}

bool TimeSlot::isMorning( ) {
    return morning;
}

bool TimeSlot::isAfternoon( ) {
    return afternoon;
}

bool TimeSlot::isEvening( ) {
    return !( morning || afternoon );
}

string TimeSlot::print(bool creditPrint) {
    string rtnVal;
    if (creditPrint) {
        stringstream strCredits;
        strCredits.precision(1);
        strCredits << fixed << credits;
        rtnVal = strCredits.str( ) + ": ";
    } else
        rtnVal = "";

    for (int i = 0; i < 6; i++) {
        if (days[i].startTime != -1) {
            rtnVal += dayTitle[i] + "(" + to_string(days[i].startTime) + ":" + to_string(days[i].endTime) + ") ";
        }
    }

    return rtnVal;
}

int TimeSlot::getStartQuarter(int day) {
    int mins = days[day].startTime % 100;
    if (mins >= 0 && mins <= 15)
        return 0;
    else if (mins >= 16 && mins <= 30)
        return 1;
    else if (mins >= 31 && mins <= 45)
        return 2;
    else
        return 3;
}

int TimeSlot::getEndQuarter(int day) {
    int mins = days[day].endTime % 100;
    if (mins >= 0 && mins <= 15)
        return 0;
    else if (mins >= 16 && mins <= 30)
        return 1;
    else if (mins >= 31 && mins <= 45)
        return 2;
    else
        return 3;
}

ostream& operator<<(ostream& os, const TimeSlot& ts) {
    os << ts.credits << ": ";
    for (int i = 0; i < 6; i++) {
        os << ", " << ts.days[i].startTime << ":" << ts.days[i].endTime;
    }
    os << ";";
    return os;
}