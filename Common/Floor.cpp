#include "Floor.h"

Floor::Floor(const vector< pair<int, int> >& indexes) {
    for(pair<int, int> index : indexes) {
        _map[index] = nullptr;
    }
}

int Floor::insert(int x, int y, std::unique_ptr<Container> container) {
    if(_map.find({x,y}) == _map.end()) {
        printf("Unavailable location for this floor");
        return LOCATION;
    }
    else if(_map[{x,y}] != nullptr) {
        printf("There is a container in this position");
        return CONTAINER;
    }
    _map[{x,y}] = std::move(container);
    return OK;
}

pair<int, unique_ptr<Container>> Floor::pop(int x, int y) {
    if(_map.find({x,y}) == _map.end()) {
        printf("Unavailable location for this floor");
        return {LOCATION, nullptr};
    }
    else if(_map[{x,y}] != nullptr) {
        unique_ptr<Container> container = std::move(_map[{x,y}]);
        _map[{x,y}] = nullptr; // TODO: will this delete container?
        return {OK, std::move(container)};
    }
    printf("No container to pop");
    return {CONTAINER, nullptr};
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

string Floor::getContainerDest(pair<int, int> location) {
    return _map[location] -> getDest();
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

bool Floor::hasContainer(string container_id) {
    for(const auto& element : _map) {
        if(element.second != nullptr && element.second->getId() == container_id) {
            return true;
        }
    }
    return false;
}
