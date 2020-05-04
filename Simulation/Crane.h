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
using  std::pair;
using std::unique_ptr;

class Crane {
    vector<Operation> _operations;
private:
    map<string, vector<unique_ptr <Container>>> _container_data;
    string _port;
    string _error_path;
    string _sail_info;

public:
    void set_operations(vector<Operation> operations);
    void set_error_path( const string &error_path);
    void set_sail_info(const string &sail_info);
    pair<int,int> start(ShipPlan& plan, ShipRoute& route, vector<unique_ptr<Container>> containers, vector<Operation> operations, const string &error_path, const string &sail_info);
    void end(ShipPlan& plan);
    pair<int, int> load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route);
    pair<int,int> unload(string id, Position pos, ShipPlan& plan);
    int reject(const string& id, int errors, ShipPlan& plan, ShipRoute& route);
    void set_container_data(vector<unique_ptr<Container>> containers);
    pair<int, int> containerNotFoundError(const string& place);
    int shouldReject(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route);
    void writeInstructionError(const string &instruction, const string& id, bool executed);
    int isErrorLoad(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route, Position pos, bool& execute);
    int isErrorUnload(const string& id, ShipPlan& plan, Position pos, bool& execute);
    void writeRejectError(const string& id, const string& msg);
};


#endif //STOWAGE_CRANE_H
