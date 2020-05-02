#ifndef STOWAGE_CRANE_H
#define STOWAGE_CRANE_H

#include <string>
#include <memory>
#include <map>
#include <iostream>
#include "../Common/Container.h"
#include "../Common/ShipPlan.h"
#include "../Common/Operation.h"
#include "../Common/Reader.h"


using std::string;
using std::map;
using std::unique_ptr;

class Crane {
    vector<Operation> _operations;
private:
    map<string, unique_ptr <Container>> _container_data;
public:
    void set_operations(vector<Operation> operations);
    int start(ShipPlan& plan);
    void end();
    bool load(const string& id, Position pos, ShipPlan& plan); //assuming container has needed location in _pos field
    bool unload(string id, Position pos, ShipPlan& plan); //assuming container has needed location in _pos field if needs to be re-loaded
    void reject(string id, Position pos);
    void set_container_data(map<string, unique_ptr <Container>> container_data);

};


#endif //STOWAGE_CRANE_H
