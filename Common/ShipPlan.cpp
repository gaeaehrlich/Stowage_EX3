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


bool ShipPlan::hasContainer(string container_id) {
    for(int i = 0; i < _floors.size(); i++) {
        if(_floors[i].hasContainer(container_id)) {
            return true;
        }
    }
    return false;
}

bool ShipPlan::isFull() {
    for(int i = 0; i < _floors.size(); i++) {
        if(!_floors[i].isFloorFull()) {
            return true;
        }
    }
    return false;
}

bool ShipPlan::isLegalLocation(Position position) {
    return position._floor >= 0 && _floors[position._floor].isLegalLocation(position._x, position._y);
}

bool ShipPlan::isEmptyPosition(Position position) {
    return position._floor >= 0 && _floors[position._floor].isEmpty(position._x, position._y);
}
