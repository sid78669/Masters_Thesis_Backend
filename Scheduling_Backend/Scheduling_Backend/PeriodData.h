#ifndef PERIODDATA
#define PERIODDATA

#include <string>
using namespace std;
class PeriodData {
private:
    int sectionID;
    int profID;
public:
    PeriodData(int _sectionID, int _profID) {
        sectionID = _sectionID;
        profID = _profID;
    }

    string printPeriod( ) {
        return "(" + to_string(sectionID) + ", " + to_string(profID) + ")";
    }

    ~PeriodData( ) {
    }
};

#endif