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
    void end();
    bool load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route, const string &error_path, const string &sail_info);
    bool unload(string id, Position pos, ShipPlan& plan, const string &error_path, const string &sail_info);
    bool reject(string id, int errors, ShipPlan& plan, ShipRoute& route, const string &error_path, const string &sail_info);
    void set_container_data(map<string, unique_ptr <Container>> container_data);
    void containerNotFoundError(const string& error_path, const string& sail_info, const string& place);
    bool shouldReject(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route);

};


#endif //STOWAGE_CRANE_H
