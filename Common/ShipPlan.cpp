#include "ShipPlan.h"


ShipPlan::ShipPlan() {
    _floors = vector<Floor>();
}

ShipPlan::ShipPlan(int num, map< pair<int,int>, int > dict) {
    vector< vector<pair<int,int>> > floors(static_cast<unsigned long long int>(num));
    map< pair<int, int> , int> ::iterator it;
    for (it = dict.begin(); it != dict.end(); it++) {
        for(int i = num - 1; i > num - (it -> second) - 1; i--) {
            floors[i].push_back(it -> first);
        }
    }
    for(const vector<pair<int,int>>& floor: floors) {
        _floors.emplace_back(floor);
    }
}

int ShipPlan::numberOfFloors() {
    return static_cast<int>(_floors.size());
}

Floor& ShipPlan::getFloor(int floor_number) {
    return _floors[floor_number];
}


bool ShipPlan::hasContainer(const string& container_id) {
    for(auto & _floor : _floors) {
        if(_floor.hasContainer(container_id)) {
            return true;
        }
    }
    return false;
}

bool ShipPlan::isFull() {
    for(auto & _floor : _floors) {
        if(!_floor.isFloorFull()) {
            return true;
        }
    }
    return false;
}

bool ShipPlan::isEmpty() {
    for(auto & _floor : _floors) {
        if(!_floor.isFloorEmpty()) {
            return false;
        }
    }
    return true;
}

bool ShipPlan::isLegalFloor(Position position) {
    return position._floor >= 0 && position._floor < numberOfFloors();
}

bool ShipPlan::isLegalXY(Position position) {
    return _floors[position._floor].isLegalLocation(position._x, position._y);
}

bool ShipPlan::isLegalLocation(Position position) {
    return isLegalFloor(position) && isLegalXY(position);
}

bool ShipPlan::isEmptyPosition(Position position) {
    return isLegalFloor(position) && _floors[position._floor].isEmpty(position._x, position._y);
}

string ShipPlan::getIdAtPosition(Position position) {
    if(!isLegalLocation(position)) {
        return "";
    }
    return _floors[position._floor].getContainerID(position._x, position._y);
}

string ShipPlan::getDestAtPosition(Position position) {
    if(!isLegalLocation(position)) {
        return "";
    }
    return _floors[position._floor].getContainerDest(position._x, position._y);
}

int ShipPlan::getWeightById(const string& id) {
    for(auto& floor: _floors) {
        if(floor.hasContainer(id)) {
            return floor.getWeightById(id);
        }
    }
    return -1;
}

int ShipPlan::getWeightByPosition(Position position) {
    return _floors[position._floor].getWeightByPosition(position._x, position._y);
}

