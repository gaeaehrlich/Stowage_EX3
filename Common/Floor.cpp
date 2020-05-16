#include "Floor.h"

Floor::Floor(const vector< pair<int, int> >& indexes) {
    for(pair<int, int> index : indexes) {
        _map[index] = nullptr;
    }
}

bool Floor::insert(int x, int y, std::unique_ptr<Container> container) {
    if(_map.find({x,y}) == _map.end()) { // illegal location
        return false;
    }
    else if(_map[{x,y}] != nullptr) { // already container at this position
        return false;
    }
    _map[{x,y}] = std::move(container);
    return true;
}

unique_ptr<Container> Floor::pop(int x, int y) {
    if(_map.find({x,y}) == _map.end()) {
        printf("Unavailable location for this floor");
        return nullptr;
    }
    else if(_map[{x,y}] != nullptr) {
        unique_ptr<Container> container = std::move(_map[{x,y}]);
        _map[{x,y}] = nullptr; // TODO: will this delete container?
        return std::move(container);
    }
    printf("No container to pop");
    return nullptr;
}

bool Floor::isEmpty(int x, int y) {
    return _map[{x,y}] == nullptr;
}

bool Floor::isEmpty(pair<int, int> location) {
    return _map[location] == nullptr;
}


vector<pair<int, int> > Floor::getLegalLocations() {
    vector<pair<int, int>> keys = vector<pair<int, int>>();
    for(const auto& it : _map) {
        keys.push_back(it.first);
    }
    return keys;
}

bool Floor::isLegalLocation(int x, int y) {
    for(const auto& it : _map) {
        if(it.first == std::make_pair(x,y)) {
            return true;
        }
    }
    return false;
}

string Floor::getContainerDest(pair<int, int> location) {
    return _map[location] ? _map[location] -> getDest() : "";
}

bool Floor::isFloorEmpty() {
    for(const auto& element : _map) {
        if(element.second != nullptr) {
            return false;
        }
    }
    return true;
}

bool Floor::isFloorFull() {
    for(const auto& element : _map) {
        if(element.second == nullptr) {
            return false;
        }
    }
    return true;
}

bool Floor::hasContainer(const string& container_id) {
    for(const auto& element : _map) {
        if(element.second != nullptr && element.second->getId() == container_id) {
            return true;
        }
    }
    return false;
}

string Floor::getContainerID(int x, int y) {
    if(!isLegalLocation(x, y)) {
        return "";
    }
    return _map[{x,y}] ? _map[{x,y}] -> getId() : "";
}

string Floor::getContainerDest(int x, int y) {
    if(!isLegalLocation(x, y)) {
        return "";
    }
    return _map[{x,y}] ? _map[{x,y}] -> getDest() : "";
}

int Floor::getWeightById(const string& id) {
    for(const auto& element : _map) {
        if(element.second != nullptr && element.second -> getId() == id) {
            return element.second -> getWeight();
        }
    }
    return -1;
}

int Floor::getWeightByPosition(int x, int y) {
    return _map[{x, y}] ? _map[{x, y}] -> getWeight() : -1;
}