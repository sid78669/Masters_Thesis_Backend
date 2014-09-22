#include "TimeSlot.h"


TimeSlot::TimeSlot(double creds) {
    credits = creds;
    dayTitle[0] = "M";
    dayTitle[1] = "T";
    dayTitle[2] = "W";
    dayTitle[3] = "R";
    dayTitle[4] = "F";
    dayTitle[5] = "S";
}

TimeSlot::TimeSlot(TimeSlot * &other) {
    dayTitle[0] = "M";
    dayTitle[1] = "T";
    dayTitle[2] = "W";
    dayTitle[3] = "R";
    dayTitle[4] = "F";
    dayTitle[5] = "S";
    credits = other->credits;
    for (int i = 0; i < 6; i++)
        days[i] = other->days[i];
}


TimeSlot::~TimeSlot( ) {
}

void TimeSlot::setPeriod(int day, string times) {
    int start = -1, end = -1;
    start = stoi(times.substr(times.find("(") + 1, times.find(":") - 1));
    end = stoi(times.substr(times.find(":") + 1, times.find(")") - 1));
    days[day].startTime = start;
    days[day].endTime = end;
}

void TimeSlot::setCredits(double newVal) {
    credits = newVal;
}

double TimeSlot::getCredits( ) {
    return credits;
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

string TimeSlot::print( ) {
    string rtnVal = to_string(credits) + ": ";

    for (int i = 0; i < 6; i++) {
        if (days[i].startTime != -1) {
            rtnVal += dayTitle[i] + "(" + to_string(days[i].startTime) + ":" + to_string(days[i].endTime) + ")";
        }
    }


    return rtnVal;
}

ostream& operator<<(ostream& os, const TimeSlot& ts) {
    os << ts.credits << ": ";
    for (int i = 0; i < 6; i++) {
        os << ", " << ts.days[i].startTime << ":" << ts.days[i].endTime;
    }
    os << ";";
    return os;
}