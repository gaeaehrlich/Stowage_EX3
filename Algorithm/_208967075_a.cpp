#include "_208967075_a.h"
REGISTER_ALGORITHM(_208967075_a)

// Idea: from bottom to top, looks for position above container with smallest diff in destination
// favor pos without a container bellow it
Position _208967075_a::findPosition(const Container& container) {
    int diff = -1, weight = container.getWeight();
    string new_dest = container.getDest();
    int new_dist = _route.portDistance(new_dest);
    Position best;
    for(int i = 0; i < _plan.numberOfFloors(); ++i) {
        Floor& floor = _plan.getFloor(i);
        for(pair<int,int> location: floor.getLegalLocations()) {
            if(floor.isEmpty(location) &&
                _calc.tryOperation(LOAD, weight, location.first, location.second) == WeightBalanceCalculator::APPROVED) {
                int old_dist = new_dist;
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
                    diff = _route.getRoute().size() + 1;
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
