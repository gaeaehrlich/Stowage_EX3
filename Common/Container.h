#ifndef STOWAGE_CONTAINER_H
#define STOWAGE_CONTAINER_H

#include <string>

using std::string;

class Container {
    int _weight;
    string _destination;
    string _id;

public:
    Container(int weight, string destination, string id);
    [[nodiscard]] string getDest() const;
    [[nodiscard]] string getId() const;
};


#endif //STOWAGE_CONTAINER_H
