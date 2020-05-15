#include "_208967075_b.h"
REGISTER_ALGORITHM(_208967075_b)


Position _208967075_b::findPosition(const Container& container) {
    int weight = container.getWeight();
    for(int i = 0; i < _plan.numberOfFloors(); ++i) {
        Floor& floor = _plan.getFloor(i);
        for(pair<int,int> location: floor.getLegalLocations()) {
            if(floor.isEmpty(location) && _calc.tryOperation(LOAD, weight, location.first, location.second) == WeightBalanceCalculator::APPROVED) {
                return Position(i, location.first, location.second);
            }
        }
    }
    return Position();
}
