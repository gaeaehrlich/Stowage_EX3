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
    bool hasContainer(string container_id);
    bool isFull();
};


#endif //STOWAGE_SHIPPLAN_H
