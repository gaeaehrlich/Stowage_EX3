#ifndef STOWAGE_OPERATION_H
#define STOWAGE_OPERATION_H

#include "Container.h"
#include "Position.h"

enum Op { LOAD = 'L', UNLOAD = 'U', REJECT = 'R' , MOVE = 'M' , ERROR};

class Operation {
public:
    Op _operation;
    string _container_id;
    Position _position;
    Position _move;

public:
    Operation(char operation, string container_id, Position position, Position move = Position());
};

#endif //STOWAGE_OPERATION_H
