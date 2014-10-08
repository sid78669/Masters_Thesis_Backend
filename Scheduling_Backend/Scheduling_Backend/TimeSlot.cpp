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
        }
        else if (start > 1200 && end < 1800) {
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