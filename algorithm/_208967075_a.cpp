#include "_208967075_a.h"
REGISTER_ALGORITHM(_208967075_a)

// Idea: from bottom to top, looks for position above container with smallest diff in destination
// favor pos without a container bellow it

Position _208967075_a::findPosition(const unique_ptr<Container>& container) {
    int diff = -_route.getRoute().size(), weight = container -> getWeight();
    int newDist = _route.portDistance(container -> getDest());
    Position best;
    for(int i = 0; i < _plan.numberOfFloors(); ++i) {
        Floor& floor = _plan.getFloor(i);
        for(pair<int,int> location: floor.getLegalLocations()) {
            if(_plan.isLegalLoadPosition(Position(i, location.first, location.second)) &&
                _calc.tryOperation(LOAD,weight, location.first, location.second) == WeightBalanceCalculator::APPROVED) {
                int oldDist = newDist;
                if (_plan.isLegalLocation(Position(i - 1, location.first, location.second))) {
                    string port = _plan.getDestAtPosition(Position(i - 1,
                                                                   location.first,
                                                                   location.second));
                    oldDist = _route.portDistance(port);
                }
                if (diff < 0 ? diff < oldDist - newDist : (oldDist >= newDist && oldDist - newDist < diff)) {
                    diff = oldDist - newDist;
                    best = Position(i, location.first, location.second);
                }
                if (diff == 0) { break; } // ideal!
            }
        }
        if (diff == 0) { break; }
    }
    return best;
}
