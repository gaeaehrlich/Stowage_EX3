#ifndef STOWAGE_SHIPPLAN_H
#define STOWAGE_SHIPPLAN_H

#include "Floor.h"
#include "Position.h"

class ShipPlan {
    vector<Floor> _floors;

public:
    ShipPlan();
    ShipPlan(int num, map< pair<int,int>, int > dict);
    int numberOfFloors();
    Floor& getFloor(int floor_number);
    bool hasContainer(const string& container_id);
    bool isFull();
    bool isEmpty();
    bool isLegalLocation(Position position);
    bool isEmptyPosition(Position position);
    bool isLegalFloor(Position position);
    bool isLegalXY(Position position);
    string getIdAtPosition(Position position);
    string getDestAtPosition(Position position);
    int getWeightById(const string& id);
    int getWeightByPosition(Position position);
};


#endif //STOWAGE_SHIPPLAN_H
