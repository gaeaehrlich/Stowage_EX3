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
    map<string, unique_ptr <Container>> _container_data;
    string _port;

public:
    void set_operations(vector<Operation> operations);
    pair<int,int> start(ShipPlan& plan, ShipRoute& route, vector<Operation> operations, const string &error_path, const string &sail_info);
    void end(ShipPlan& plan, const string &error_path, const string &sail_info);
    pair<int, int> load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route, const string &error_path, const string &sail_info);
    pair<int,int> unload(string id, Position pos, ShipPlan& plan, const string &error_path, const string &sail_info);
    int reject(const string& id, int errors, ShipPlan& plan, ShipRoute& route, const string &error_path, const string &sail_info);
    void set_container_data(map<string, unique_ptr <Container>> container_data);
    pair<int, int> containerNotFoundError(const string& error_path, const string& sail_info, const string& place);
    int shouldReject(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route);
    void writeInstructionError(const string& error_path, const string& sail_info, const string &instruction, const string& id, bool executed);
    int isErrorLoad(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route, Position pos, const string& error_path, const string& sail_info, bool& execute);
    int isErrorUnload(const string& id, ShipPlan& plan, Position pos, const string& error_path, const string& sail_info, bool& execute);
};


#endif //STOWAGE_CRANE_H
