#include "Crane.h"
#include "Simulation.h"

void Crane::setContainerData(vector<unique_ptr<Container>> containers) {
    for(auto& container: containers) {
        _containerData[container -> getId()].emplace_back(std::move(container));
    }
}

void Crane::setOperations(vector<Operation> operations) {
    _operations = std::move(operations);
}

void Crane::setErrorPath(const string &errorPath) {
    _errorPath = errorPath;
}

void Crane::setSailInfo(const string &sailInfo) {
    _sailInfo = sailInfo;
}


void Crane::setCalculator(WeightBalanceCalculator &calculator) {
    _calculator = calculator;
}

int Crane::start(ShipPlan& plan, ShipRoute& route, WeightBalanceCalculator& calculator, vector<unique_ptr<Container>> containers, vector<Operation> operations, const string &errorPath, const string &sailInfo) {
    setContainerData(std::move(containers));
    setOperations(std::move(operations));
    setErrorPath(errorPath);
    setSailInfo(sailInfo);
    setCalculator(calculator);
    _port = route.getCurrentPort();
    _errorPort = false;
    int sum_operations = 0;
    bool flag = false;
    for(Operation& op : _operations) {
        switch(op._operation) {
            case LOAD:
                if(!load(op._id, op._position, plan, route)) { flag = true; }
                sum_operations++;
                break;
            case UNLOAD:
                if(!unload(op._id, op._position, plan)) { flag = true; }
                sum_operations++;
                break;
            case REJECT:
                if(!reject(op._id, plan, route)) { flag = true; }
                break;
            case MOVE:
                if(!unload(op._id, op._position, plan) || !load(op._id, op._move, plan, route)) {  flag = true; }
                sum_operations++;
                break;
            case ERROR:
                std::ofstream file;
                file.open(_errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
                file << _sailInfo << "ERROR: Algorithm trying an illegal operation.\n";
                file.close();
                break;
        }
    }
    if(!end(plan, route)) { flag = true; }
    return flag ? FAILURE : sum_operations;
}

bool Crane::end(ShipPlan& plan, ShipRoute& route) {
    bool isLegal = checkLoadedTemporaryUnloaded() && checkWronglyUnloaded(plan, route) && checkShip(plan) && handleLastStop(plan, route);
    _containerData.clear();
    _temporaryUnloaded.clear();
    _newlyLoadedDest.clear();
    return isLegal;
}

bool Crane::load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route) {
    unique_ptr<Container> container;
    if(_containerData[id].empty()) {
        containerNotFoundError("at port");
        return false;
    }
    container = std::move(_containerData[id][0]);
    _containerData[id].erase(_containerData[id].begin());

    bool fatal = false;
    bool isError = isErrorLoad(container, plan, route, pos, fatal);
    if(fatal) { return false; }

    if(_calculator.tryOperation(LOAD, container -> getWeight(), pos._x, pos._y) != WeightBalanceCalculator::APPROVED) { return false; }

    if(_temporaryUnloaded.find(container -> getId()) != _temporaryUnloaded.end()) {
        _temporaryUnloaded.erase(container -> getId());
    }
    else {
        _newlyLoadedDest.insert(container -> getDest());
    }

    plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container));
    std::cout << "Loading container " << id << " to position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _containerData.erase(id);
    return !isError;
}

bool Crane::unload(const string& id, Position pos, ShipPlan& plan) {
    bool fatal = false;
    bool isError = isErrorUnload(id, plan, pos, fatal);
    if(fatal) { return false; }

    if(_calculator.tryOperation(UNLOAD, plan.getWeightById(id), pos._x, pos._y) != WeightBalanceCalculator::APPROVED) { return false; }

    unique_ptr<Container> removed = std::move(plan.getFloor(pos._floor).pop(pos._x, pos._y));
    if(removed -> getDest() != _port) {
        _temporaryUnloaded.insert(removed -> getId());
    }
    std::cout << "Unloading container " << id << " from position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _containerData[removed -> getId()].emplace_back(std::move(removed));
    return !isError;
}

bool Crane::reject(const string& id, ShipPlan& plan, ShipRoute& route) {
    unique_ptr<Container> container;
    if(!_containerData[id].empty()) {
        container = std::move(_containerData[id][0]);
        _containerData[id].erase(_containerData[id].begin());
    }
    else {
        containerNotFoundError("at port");
        return false;
    }
    if(shouldReject(container, plan, route) == 0) {
        writeInstructionError("Reject", container -> getId(), true);
        return false;
    }
    std::cout << "Rejecting container " << id << std::endl;
    return true;
}

