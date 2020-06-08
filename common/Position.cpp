#include "Position.h"

Position::Position(int floor, int x, int y) {
    _floor = floor;
    _x = x;
    _y = y;
}

Position::Position() {
    _floor =  -1;
    _x = -1;
    _y = -1;
}

bool Position::operator!=(const Position &other) const {
    return _floor != other._floor || _x != other._x || _y != other._y;
}
