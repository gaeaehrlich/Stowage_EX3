#ifndef STOWAGE_FLOOR_H
#define STOWAGE_FLOOR_H

#include "Container.h"
#include <map>
#include <vector>
#include <memory>

using std::map;
using std::vector;
using std::pair;
using std::unique_ptr;

class Floor {
    map< pair<int, int> , std::unique_ptr<Container>> _map;

public:
    Floor();
    explicit Floor(const vector< pair<int,int> >& indexes);
    bool insert(int x, int y, std::unique_ptr<Container> container);
    unique_ptr<Container> pop(int x, int y);
    bool isEmpty(int x, int y);
    bool isEmpty(pair<int, int> location);
    vector< pair<int, int> > getLegalLocations();
    string getContainerDest(pair<int,int> location);
    bool isFloorEmpty();
    bool isFloorFull();
    bool hasContainer(const string& container_id);
    bool isLegalLocation(int x, int y);
    string getContainerID(int x, int y);
    string getContainerDest(int x, int y);
    int getWeightById(const string& id);
};


#endif //STOWAGE_FLOOR_H
