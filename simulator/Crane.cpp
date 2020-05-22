#include "Crane.h"
#include "Simulation.h"


int Crane::start(ShipPlan& plan, ShipRoute& route, WeightBalanceCalculator& calculator, vector<unique_ptr<Container>> containers, vector<Operation> operations, const string &errorPath, const string &sailInfo) {
    setCrane(std::move(containers), std::move(operations), errorPath, sailInfo, calculator, route);
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
                file << _sailInfo << "ERROR: algorithm trying an illegal operation.\n";
                file.close();
                break;
        }
    }
    if(!end(plan, route)) { flag = true; }
    return flag ? FAILURE : sum_operations;
}


void Crane::setCrane(vector<unique_ptr<Container>> containers, vector<Operation> operations, const string &errorPath,
                     const string &sailInfo, WeightBalanceCalculator &calculator, ShipRoute &route) {
    setContainerData(std::move(containers));
    setOperations(std::move(operations));
    setErrorPath(errorPath);
    setSailInfo(sailInfo);
    setCalculator(calculator);
    _port = route.getCurrentPort();
    _errorPort = false;
}

bool Crane::end(ShipPlan& plan, ShipRoute& route) {
    bool isLegal = checkLoadedTemporaryUnloaded() && checkForgotOnPort(route.isLastStop()) && checkShip(plan) && handleLastStop(plan, route);
    _cargoLoad.clear();
    _temporaryUnloaded.clear();
    _newlyLoadedDest.clear();
    return isLegal;
}


unique_ptr<Container> Crane::getContainerToLoad(const string &id) {
    auto cargoLoadPointer = find_if(_cargoLoad.begin(), _cargoLoad.end(), [&](unique_ptr<Container>& container) {
        return container -> getId() == id;
    });
    auto temporaryUnloadedPointer = find_if(_temporaryUnloaded.begin(), _temporaryUnloaded.end(), [&](unique_ptr<Container>& container) {
        return container -> getId() == id;
    });
    if(temporaryUnloadedPointer != _temporaryUnloaded.end()) {
        auto container = std::move(*temporaryUnloadedPointer);
        _temporaryUnloaded.erase(temporaryUnloadedPointer);
        return container;
    }
    else if(cargoLoadPointer != _cargoLoad.end()) {
        auto container = std::move(*cargoLoadPointer);
        _cargoLoad.erase(cargoLoadPointer);
        _newlyLoadedDest.insert(container -> getDest());
        return container;
    }
    else if(isInDuplicated(id)) {
        writeLoadError(id, "This container is a duplicate ID on port and should have been rejected. Only the first container with this ID is taken under consideration.\n");
        writeInstructionError("Load", id, false);
        return nullptr;
    }
    else  {
        containerNotFoundError("at port");
        return nullptr;
    }
}


bool Crane::load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route) {
    unique_ptr<Container> container = getContainerToLoad(id);
    if(!container) { return false; }

    bool fatal = false;
    bool isError = isErrorLoad(container, plan, route, pos, fatal);
    if(fatal) { return false; }

    if(_calculator.tryOperation(LOAD, container -> getWeight(), pos._x, pos._y) != WeightBalanceCalculator::APPROVED) { return false; }

    plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container));
    std::cout << "Loading container " << id << " to position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    return !isError;
}

bool Crane::unload(const string& id, Position pos, ShipPlan& plan) {
    bool fatal = false;
    bool isError = isErrorUnload(id, plan, pos, fatal);
    if(fatal) { return false; }

    if(_calculator.tryOperation(UNLOAD, plan.getWeightById(id), pos._x, pos._y) != WeightBalanceCalculator::APPROVED) { return false; }

    unique_ptr<Container> removed = std::move(plan.getFloor(pos._floor).pop(pos._x, pos._y));
    if(removed -> getDest() != _port) {
        _temporaryUnloaded.emplace_back(std::move(removed));
    }
    std::cout << "Unloading container " << id << " from position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    return !isError;
}

unique_ptr<Container> Crane::getContainerToReject(const string &id) {
    auto duplicatePointer = find_if(_duplicates.begin(), _duplicates.end(), [&](unique_ptr<Container>& container) {
        return container -> getId() == id;
    });
    auto cargoLoadPointer = find_if(_cargoLoad.begin(), _cargoLoad.end(), [&](unique_ptr<Container>& container) {
        return container -> getId() == id;
    });

    if(duplicatePointer != _duplicates.end()) {
        auto container = std::move(*duplicatePointer);
        _duplicates.erase(duplicatePointer);
        return container;
    }
    else if(cargoLoadPointer != _cargoLoad.end()) {
        auto container = std::move(*cargoLoadPointer);
        _cargoLoad.erase(cargoLoadPointer);
        return container;
    }
    else if(isInTemporaryUnloaded(id)) {
        writeInstructionError("Reject", id, false);
        return nullptr;
    }
    else  {
        containerNotFoundError("at port");
        return nullptr;
    }
}

bool Crane::reject(const string& id, ShipPlan& plan, ShipRoute& route) {
    bool duplicate = isDuplicateOnPort(id);
    unique_ptr<Container> container = getContainerToReject(id);
    if(!container) { return false; }

    if(!duplicate && shouldReject(container, plan, route) == 0) {
        writeInstructionError("Reject", container -> getId(), true);
        return false;
    }
    std::cout << "Rejecting container " << id << std::endl;
    return true;
}

