#ifndef STOWAGE_CRANE_H
#define STOWAGE_CRANE_H

#include <string>
#include <memory>
#include <map>
#include <iostream>
#include <unordered_set>
#include "../Common/Container.h"
#include "../Common/ShipPlan.h"
#include "../Common/Operation.h"
#include "../Common/Reader.h"


using std::string;
using std::map;
using std::pair;
using std::unique_ptr;

class Crane {
    vector<Operation> _operations;
private:
    map<string, vector<unique_ptr <Container>>> _container_data;
    std::unordered_set<string> _temporary_unloaded;
    std::unordered_set<string> _newly_loaded_dest;
    string _port;
    string _error_path;
    string _sail_info;

public:
    void set_operations(vector<Operation> operations);
    void set_error_path( const string &error_path);
    void set_sail_info(const string &sail_info);
    int start(ShipPlan& plan, ShipRoute& route, vector<unique_ptr<Container>> containers, vector<Operation> operations, const string &error_path, const string &sail_info);
    bool end(ShipPlan& plan, ShipRoute& route);
    bool load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route);
    bool unload(const string& id, Position pos, ShipPlan& plan);
    bool reject(const string& id, ShipPlan& plan, ShipRoute& route);
    void set_container_data(vector<unique_ptr<Container>> containers);
    void containerNotFoundError(const string& place);
    int shouldReject(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route, bool write = false);
    void writeInstructionError(const string &instruction, const string& id, bool executed);
    bool isErrorLoad(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route, Position pos, bool& fatal);
    bool isErrorUnload(const string& id, ShipPlan& plan, Position pos, bool& fatal);
    bool checkLoadedTemporaryUnloaded();
    void writeLeftAtPortError(const string& id, const string& msg);
    bool checkWronglyUnloaded(ShipPlan& plan, ShipRoute& route);
    bool shouldPrioritize(const string& dest, ShipRoute& route);
    void writeLoadError(const string& id, const string& reason);
    bool checkShip(ShipPlan& plan);
};


#endif //STOWAGE_CRANE_H
