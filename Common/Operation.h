#ifndef STOWAGE_OPERATION_H
#define STOWAGE_OPERATION_H

#include "Container.h"
#include "Position.h"

enum Op { LOAD, UNLOAD, REJECT };

class Operation {
public:
    Op _operation;
    string _container_id;
    Position _position;

public:
    Operation(char operation, string container_id, Position _position);
};

#endif //STOWAGE_OPERATION_H
