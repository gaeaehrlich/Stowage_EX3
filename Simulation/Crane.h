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
#include "../Common/WeightBalanceCalculator.h"


using std::string;
using std::map;
using std::pair;
using std::unique_ptr;

class Crane {
    vector<Operation> _operations;
private:
    map<string, vector<unique_ptr<Container>>> _containerData;
    std::unordered_set<string> _temporaryUnloaded;
    std::unordered_set<string> _newlyLoadedDest;
    WeightBalanceCalculator _calculator;
    string _port;
    string _errorPath;
    string _sailInfo;

public:
    void setOperations(vector<Operation> operations);
    void setErrorPath(const string &errorPath);
    void setSailInfo(const string &sailInfo);
    int start(ShipPlan& plan, ShipRoute& route, WeightBalanceCalculator& calculator, vector<unique_ptr<Container>> containers, vector<Operation> operations, const string &errorPath, const string &sailInfo);
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
    bool checkWronglyUnloaded(ShipPlan& plan, ShipRoute& route);
    bool shouldPrioritize(const string& dest, ShipRoute& route);
    void writeLoadError(const string& id, const string& reason);
    bool checkShip(ShipPlan& plan);
    bool handleLastStop(ShipPlan& plan, ShipRoute& route);
    void setCalculator(WeightBalanceCalculator& calculator);
};


#endif //STOWAGE_CRANE_H
