#ifndef STOWAGE_CRANE_H
#define STOWAGE_CRANE_H

#include <string>
#include <memory>
#include <map>
#include <iostream>
#include <unordered_set>
#include "../common/Container.h"
#include "../common/ShipPlan.h"
#include "../common/Operation.h"
#include "../common/Reader.h"
#include "../common/WeightBalanceCalculator.h"


using std::string;
using std::map;
using std::pair;
using std::unique_ptr;

class Crane {
    vector<Operation> _operations;
    vector<unique_ptr<Container>> _cargoLoad;
    vector<unique_ptr<Container>> _temporaryUnloaded;
    vector<unique_ptr<Container>> _duplicates;
    std::unordered_set<string> _newlyLoadedDest;
    WeightBalanceCalculator _calculator;
    string _port;
    pair<string, string> _sailInfo; // algorithm name, trave name
    string _craneErrors; // algorithm name, travel name, errors

public:
    void setCrane(vector<unique_ptr<Container>> containers, vector<Operation> operations, const  pair<string, string> &sailInfo, WeightBalanceCalculator& calculator, ShipRoute& route);
    void setOperations(vector<Operation> operations);
    void setSailInfo(const pair<string, string> &sailInfo);
    int start(ShipPlan& plan, ShipRoute& route, WeightBalanceCalculator& calculator, vector<unique_ptr<Container>> containers, vector<Operation> operations, const  pair<string, string> &sailInfo);
    bool end(ShipPlan& plan, ShipRoute& route);
    bool load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route);
    bool unload(const string& id, Position pos, ShipPlan& plan);
    bool reject(const string& id, ShipPlan& plan, ShipRoute& route);
    void setContainerData(vector<unique_ptr<Container>> containers);
    void containerNotFoundError(const string& place);
    int shouldReject(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route, bool write = false);
    void writeInstructionError(const string &instruction, const string& id, bool executed);
    bool isErrorLoad(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route, Position pos, bool& fatal);
    bool isErrorUnload(const string& id, ShipPlan& plan, Position pos, bool& fatal);
    bool checkLoadedTemporaryUnloaded();
    void writeLeftAtPortError(const string& id, const string& msg);
    bool checkForgotOnPort(bool isLastStop);
    bool shouldPrioritize(const string& dest, ShipRoute& route);
    void writeLoadError(const string& id, const string& reason);
    bool checkShip(ShipPlan& plan);
    bool handleLastStop(ShipPlan& plan, ShipRoute& route);
    void setCalculator(WeightBalanceCalculator& calculator);
    unique_ptr<Container> getContainerToLoad(const string& id);
    bool isDuplicateOnPort(const string& id);
    bool isInDuplicated(const string& id);
    bool isInTemporaryUnloaded(const string& id);
    unique_ptr<Container> getContainerToReject(const string& id);
    string getCraneErrors();

};


#endif //STOWAGE_CRANE_H
