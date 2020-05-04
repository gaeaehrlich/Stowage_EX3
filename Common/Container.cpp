#include "Container.h"

Container::Container(int weight, string destination, string id):
        _weight(weight), _destination(std::move(destination)), _id(std::move(id)) {}

string Container::getDest() const{
    return _destination;
}

string Container::getId() const {
    return _id;
}

int Container::getWeight() const {
    return _weight;
}

