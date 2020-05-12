#include "Operation.h"

#include <utility>

Operation::Operation(char operation, string container_id, Position position, Position move):
    _container_id(std::move(container_id)), _position(position), _move(move){
    if(operation == 'L') {
        _operation = LOAD;
    }
    else if(operation == 'U') {
        _operation = UNLOAD;
    }
    else if (operation == 'R'){
        _operation = REJECT;
    }
    else if (operation == 'M') {
        _operation = MOVE;
    }
    //else illegal operation
}
