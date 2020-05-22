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
