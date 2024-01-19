#ifndef ROW_H
#define ROW_H

#include <string>

using namespace std;

class Row {
public:
    string dateAndTime_inUtc;
    string startingLocation_city;
    string startingLocation_coordinates;
    string landingLocation;
    string distanceTraveled_km;

    Row() = default;
};

#endif
