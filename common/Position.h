#ifndef STOWAGE_POSITION_H
#define STOWAGE_POSITION_H

enum Reason { SPACE = -1, ID = -2, DEST = -3 , SYMBOL = -4, CURR = -5, EXISTS = -6 };

class Position {
public:
    int _floor;
    int _x;
    int _y;

public:
    Position();
    Position(int floor, int x, int y);
    bool operator!=(const Position& other) const;
};


#endif //STOWAGE_POSITION_H
