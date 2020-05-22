#include "Operation.h"

#include <utility>

Operation::Operation(char operation, string id, Position position, Position move):
        _id(std::move(id)), _position(position), _move(move){
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
    else {
        _operation = ERROR;
    }
}
