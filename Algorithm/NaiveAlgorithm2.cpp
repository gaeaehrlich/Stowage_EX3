//
// Created by Chen Hendler on 5/14/2020.
//

#include "NaiveAlgorithm2.h"

// Idea: from top to bottom, looks for position above container with smallest diff in destination
// does not favor pos without a container bellow it
Position NaiveAlgorithm2::findPosition(const string& new_dest) {
        int max = _route.getRoute().size() + 1, diff = -1;
        int new_dist = _route.portDistance(new_dest);
        Position best;
        for(int i = _plan.numberOfFloors() - 1; i >= 0; --i) {
            Floor& floor = _plan.getFloor(i);
            for(pair<int,int> location: floor.getLegalLocations()) {
                if(floor.isEmpty(location)) {
                    int old_dist = max + new_dist;
                    if (_plan.isLegalLocation(Position(i - 1, location.first, location.second))) {
                        string port = _plan.getDestAtPosition(Position(i - 1,
                                                                       location.first,
                                                                       location.second));
                        if (port != "") { old_dist = _route.portDistance(port); }
                    }
                    if (old_dist - new_dist  < diff && old_dist >= new_dist) {
                        diff = old_dist - new_dist;
                        best = Position(i, location.first, location.second);
                    }
                    else if (diff == -1) { // in case every position sucks
                        diff = max + 1;
                        best = Position(i, location.first, location.second);
                    }
                    if (diff == 0) { break; }
                    // TODO: what is the scope of retrun value ?
                }
            }
            if (diff == 0) { break; }
        }
        return best;
}
