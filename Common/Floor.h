#ifndef STOWAGE_FLOOR_H
#define STOWAGE_FLOOR_H

#include "Container.h"
#include <map>
#include <vector>
#include <memory>

using std::map;
using std::vector;
using std::pair;

class Floor {
    map< pair<int, int> , std::unique_ptr<Container>> _map;

public:
    Floor();
    explicit Floor(const vector< pair<int,int> >& indexes);
    bool insert(int x, int y, std::unique_ptr<Container> container);
    std::unique_ptr<Container> pop(int x, int y);
    bool isEmpty(int x, int y);
    bool isEmpty(pair<int, int> location);
    vector< pair<int, int> > getLegalLocations();
    string getContainerDest(pair<int,int> location);
    bool isFloorEmpty();
    bool isFloorFull();
    bool hasContainer(string container_id);
};


#endif //STOWAGE_FLOOR_H