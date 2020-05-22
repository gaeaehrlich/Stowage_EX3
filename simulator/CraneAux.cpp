#include "Crane.h"


void Crane::setContainerData(vector<unique_ptr<Container>> containers) {
    for(auto& container: containers) {
        if(isDuplicateOnPort(container -> getId())) {
            _duplicates.emplace_back(std::move(container));
        }
        else {
            _cargoLoad.emplace_back(std::move(container));
        }
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

bool Crane::isDuplicateOnPort(const string& id) {
    for(const auto& container: _cargoLoad) {
        if(container -> getId() == id) {
            return true;
        }
    }
    return false;
}

bool Crane::isInDuplicated(const string& id) {
    for(const auto& container: _duplicates) {
        if(container -> getId() == id) {
            return true;
        }
    }
    return false;
}

bool Crane::isInTemporaryUnloaded(const string& id) {
    for(const auto& container: _temporaryUnloaded) {
        if(container -> getId() == id) {
            return true;
        }
    }
    return false;
}

void Crane::checkErrorPort(std::ofstream& file) {
    if(!_errorPort) {
        file << _sailInfo;
        _errorPort = true;
    }
}

void Crane::containerNotFoundError(const string &place) {
    std::ofstream file;
    file.open(_errorPath, std::ios::out | std::ios::app);
    checkErrorPort(file);
    file << "Trying to get container that's not " << place << ". Instruction terminated.\n";
    file.close();
}

int Crane::shouldReject(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route, bool write) {
    int errors = 0;
    if(route.isLastStop()) {
        if(write) { writeLoadError(container -> getId(), "The container to load is at the last stop.\n"); }
        errors++;
    }
    if(plan.hasContainer(container -> getId())) {
        if(write) { writeLoadError(container -> getId(), "The ship plan already has a container with this ID.\n"); }
        errors++;
    }
    if(container -> getWeight() <= 0) {
        if(write) { writeLoadError(container -> getId(), "The container's weight is illegal.\n"); }
        errors++;
    }
    if(!Reader::legalPortSymbol(container -> getDest())) {
        if(write) { writeLoadError(container -> getId(), "The container's destination port symbol is illegal.\n"); }
        errors++;
    }
    if(!Reader::legalContainerId(container -> getId())) {
        if (write) { writeLoadError(container->getId(), "The container's ID is not in format.\n"); }
        errors++;
    }
    if(container -> getId().empty()) {
        if(write) { writeLoadError(container -> getId(), "The container's ID is illegal.\n"); }
        errors++;
    }
    if(plan.isFull() && !shouldPrioritize(container -> getDest(), route)) errors++; // this is a fatal error for load
    if(!route.portInRoute(container -> getDest())) {
        if(write) { writeLoadError(container -> getId(), "The container's destination port is not in route.\n"); }
        errors++;
    }
    if(container -> getDest() == _port) {
        if(write) { writeLoadError(container -> getId(), "The container's destination port is this port.\n"); }
        errors++;
    }
    return errors;
}

void Crane::writeLoadError(const string& id, const string& reason) {
    std::ofstream file;
    file.open(_errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    checkErrorPort(file);
    file << "ALGORITHM ERROR: algorithm is loading the container " << id << " that should be rejected. Rejection reason: " << reason;
    file.close();
}


void Crane::writeInstructionError(const string &instruction, const string &id, bool executed) {
    std::ofstream file;
    file.open(_errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    string exec = executed ? "still" : "not";
    checkErrorPort(file);
    file << "ALGORITHM ERROR: algorithm is making a mistake with container " << id << ". " << instruction << " was " << exec << " executed.\n";
    file.close();
}


bool Crane::isErrorLoad(unique_ptr<Container> &container, ShipPlan& plan, ShipRoute& route, Position pos, bool& fatal) {
    if(!plan.isLegalLoadPosition(pos)) {
        writeInstructionError("Load", container -> getId(), false);
        fatal = true;
        return true;
    }
    if(shouldReject(container, plan, route, true) > 0) {
        writeInstructionError("Load", container -> getId(), true);
        return true;
    }
    return false;
}


bool Crane::isErrorUnload(const string& id, ShipPlan &plan, Position pos, bool& fatal) {
    bool isLegalLocation = plan.isLegalLocation(pos);
    Position aboveFloor = Position(pos._floor + 1, pos._x, pos._y);
    bool cellAboveNull = pos._floor + 1 == plan.numberOfFloors() ? true : plan.isLegalFloor(aboveFloor) && plan.isEmptyPosition(aboveFloor);

    if(!plan.hasContainer(id) || !isLegalLocation || !cellAboveNull) {
        writeInstructionError("Unload", id, false);
        fatal = true;
        return true;
    }

    if(plan.getIdAtPosition(pos) != id) {
        std::ofstream file;
        file.open(_errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
        checkErrorPort(file);
        file << "ALGORITHM ERROR: algorithm is unloading the container " << plan.getIdAtPosition(pos) << " instead of " << id << ".\n";
        file.close();
        writeInstructionError("Unload", id, true);
        return true;
    }
    return false;
}

void Crane::writeLeftAtPortError(const string& id, const string& msg) {
    std::ofstream file;
    file.open(_errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    checkErrorPort(file);
    file << "ALGORITHM ERROR: algorithm is making a mistake with container " << id
         << ". It " << msg << ", and was wrongly left at port " << _port << ".\n";
    file.close();
}

bool Crane::checkForgotOnPort(bool isLastStop) {
    if(isLastStop) { return true; }
    bool flag = true;
    for(const auto& container : _cargoLoad) {
        writeLeftAtPortError(container -> getId(), "wasn't handled by the algorithm");
        flag = false;
    }
    for(const auto& container: _duplicates) {
        writeLeftAtPortError(container -> getId(), "wasn't handled by the algorithm");
        flag = false;
    }
    return flag;
}


bool Crane::shouldPrioritize(const string &dest, ShipRoute& route) {
    for(const string& port: _newlyLoadedDest) {
        if(route.isStopAfter(port, dest)) {
            return true;
        }
    }
    return false;
}

bool Crane::checkLoadedTemporaryUnloaded() {
    bool flag = true;
    if(!_temporaryUnloaded.empty()) {
        for(const auto& container: _temporaryUnloaded) {
            writeLeftAtPortError(container -> getId(), "was temporary unloaded");
        }
        flag = false;
    }
    return flag;
}


bool Crane::checkShip(ShipPlan &plan) {
    bool flag = plan.findContainersToUnload(_port).size() == 0;
    if(!flag) {
        std::ofstream file;
        file.open(_errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
        checkErrorPort(file);
        file << "ALGORITHM ERROR: The handling of port " << _port << " is finished, but there are still containers on the ship that need to be unloaded at this port.\n";
        file.close();
    }
    return flag;
}

bool Crane::handleLastStop(ShipPlan &plan, ShipRoute &route) {
    bool flag = !(route.isLastStop() && !plan.isEmpty());
    if(!flag) {
        std::ofstream file;
        file.open(_errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
        checkErrorPort(file);
        file << "ALGORITHM ERROR: The handling of the last port in route is finished, but there are still containers on the ship.\n";
        file.close();
    }
    return flag;
}