#include "_208967075_a.h"
REGISTER_ALGORITHM(_208967075_a)

// Idea: from bottom to top, looks for position above container with smallest diff in destination
// favor pos without a container bellow it

bool _208967075_a::tryMoveFrom(unique_ptr<Container> &container, const Position old, std::ofstream &file){
    Position newPosition = findPosition(container, {old._x, old._y}, true);
    if(newPosition != Position()) {
        file << instructionToString('M', container->getId(), old, newPosition);
        _plan.getFloor(newPosition._floor).insert(newPosition._x, newPosition._y, std::move(container));
        return true;
    }
    return false;
}

