#include "ShipPlan.h"


ShipPlan::ShipPlan() {
    _floors = vector<Floor>();
}

ShipPlan::ShipPlan(int num, map< pair<int,int>, int > dict) {
    vector< vector<pair<int,int>> > floors(static_cast<unsigned long long int>(num));
    map< pair<int, int> , int> ::iterator it;
    for (it = dict.begin(); it != dict.end(); it++) {
        for(int i = num - 1; i >= num - (it -> second); i--) {
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


bool ShipPlan::hasContainer(const string& id) {
    for(auto & _floor : _floors) {
        if(_floor.hasContainer(id)) {
            return true;
        }
    }
    return false;
}

bool ShipPlan::isFull() {
    for(auto & _floor : _floors) {
        if(!_floor.isFloorFull()) {
            return false;
        }
    }
    return true;
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

bool ShipPlan::isLegalLoadPosition(Position position) {
    Position lowerFloor = Position(position._floor - 1, position._x, position._y);
    bool isLegalPosition =  isLegalLocation(position) && isEmptyPosition(position);
    bool cellBelowNull = position._floor > 0 ? (this -> isLegalLocation(lowerFloor) && isEmptyPosition(lowerFloor)) : false;
    if (!(isLegalPosition && !cellBelowNull)) {
        std::cout  << "NOT EMPTY - POSITION " << position._floor << position._x << position._y << " has "<< getIdAtPosition(position) << std::endl;
    }
    return isLegalPosition && !cellBelowNull;
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

int ShipPlan::numberOfEmptyCells() {
    int count = 0;
    for(auto & _floor : _floors) {
        count += _floor.numberOfEmptyCells();
    }
    return count;
}

vector<Position> ShipPlan::findContainersToUnload(const string& port) {
    vector<Position> unload;
    for(int i = numberOfFloors() - 1; i >= 0 ; i--) {
        Floor& floor = getFloor(i);
        for(pair<int,int> location: floor.getLegalLocations()) {
            if(!floor.isEmpty(location) && floor.getContainerDest(location) == port) {
                unload.emplace_back(Position(i, location.first, location.second));
            }
        }
    }
    return unload;
}

