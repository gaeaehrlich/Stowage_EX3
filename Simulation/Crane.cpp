#include "Crane.h"

void Crane::set_operations(vector<Operation> operations) {
    _operations = std::move(operations);
}

void Crane::set_error_path( const string &error_path) {
    _error_path = error_path;
}

void Crane::set_sail_info(const string &sail_info) {
    _sail_info = sail_info;
}

pair<int,int> Crane::start(ShipPlan& plan, ShipRoute& route, vector<unique_ptr<Container>> containers, vector<Operation> operations, const string &error_path, const string &sail_info) {
    set_container_data(std::move(containers));
    set_operations(std::move(operations));
    set_error_path(error_path);
    set_sail_info(sail_info);
    int succ = 0, err = 0;
    pair<int, int> result;
    for(Operation& op : _operations) {
        switch(op._operation) {
            case LOAD:
                result = load(op._container_id, op._position, plan, route);
                succ += result.first;
                err += result.second;
                break;
            case UNLOAD:
                result = unload(op._container_id, op._position, plan);
                succ += result.first;
                err += result.second;
                break;
            case REJECT: //reject assuming error code is in floor
                err += reject(op._container_id, op._position._floor, plan, route);
                break;
        }
    }
    end(plan);
    return {succ, err};
}

void Crane::end(ShipPlan& plan) {
    std::ofstream file;
    for(auto& element : _container_data) {
        for(auto& container: element.second) {
            if ((container->getDest() != _port) && !plan.isFull()) {
                file.open(_error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
                file << _sail_info << "ERROR: Algorithm is making a mistake with container " << container->getId()
                     << ". It was wrongly left at port " << _port << ".\n";
                file.close();
            }
        }
    }
    _container_data.clear();
}

pair<int,int> Crane::load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route) {
    unique_ptr<Container> container;
    if(_container_data.count(id) == 0) {
        return containerNotFoundError("at port");
    }
    container = std::move(_container_data[id][0]);
    _container_data[id].erase(_container_data[id].begin());

    bool execute = true;
    int errors = isErrorLoad(container, plan, route, pos, execute);
    if(!execute) { return {0, errors}; }

    plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container));
    std::cout << "Loading container " << id << " to position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data.erase(id);
    return {1, errors};
}

pair<int,int> Crane::unload(string id, Position pos, ShipPlan& plan) { // TODO: if making error, do we want the instruction to be made? what about _cargo_data?
    bool execute = true;
    int errors = isErrorUnload(id, plan, pos, execute);
    if(!execute) { return {0, errors}; }

    pair<int, unique_ptr<Container>> removed = std::move(plan.getFloor(pos._floor).pop(pos._x, pos._y));
    std::cout << "Unloading container " << id << " from position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data[removed.second -> getId()].emplace_back(std::move(removed.second));
    return {1, errors};
}

// return: true if rejecting, false if not
int Crane::reject(const string& id, int errors, ShipPlan& plan, ShipRoute& route) {
    unique_ptr<Container> container;
    if(_container_data.count(id) >= 1) {
        container = std::move(_container_data[id][0]);
        _container_data[id].erase(_container_data[id].begin());
    }
    else {
        containerNotFoundError("at port");
        return false;
    }
    if(errors == 0) { return false; } //TODO: algorithm didn't give a reject reason
    int error_count = 0;
    if((errors & 10) && (_container_data.count(id) == 0)) {
        writeRejectError(id, "containers at port: duplicate ID on port (ID rejected)\n");
        error_count++;
    }
    if((errors & 11) && (!plan.hasContainer(id))) {
        writeRejectError(id, "containers at port: ID already on ship (ID rejected)\n");
        error_count++;
    }
    if((errors & 12) && (container -> getWeight() >= 0)) {
        writeRejectError(id, "containers at port: bad line format, missing or bad weight (ID rejected)\n");
        error_count++;
    }
    // TODO: should the 2 last reasons for 13 be here?
    if((errors & 13) && (Reader::legalPortSymbol(container -> getDest())
                        || route.portInRoute(container -> getDest())
                        || (container-> getDest() != _port))) {
        writeRejectError(id, "containers at port: bad line format, missing or bad port dest (ID rejected)\n");
        error_count++;
    }
    if((errors & 14) && (!container -> getId().empty())){
        writeRejectError(id, "containers at port: bad line format, ID cannot be read (ignored)\n");
        error_count++;
    }
    if((errors & 15) && (Reader::legalContainerId(id))) {
        writeRejectError(id, "containers at port: illegal ID check ISO 6346 (ID rejected)\n");
        error_count++;
    }
    if(errors & 16) {
        //TODO
        writeRejectError(id, "containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n");
        error_count++;
    }
    if((errors & 17) && (!route.isLastStop())) {
        writeRejectError(id, "containers at port: last port has waiting containers (ignored)\n");
        error_count++;
    }
    if((errors & 18) && (!plan.isFull())) {
        writeRejectError(id, "containers at port: total containers amount exceeds ship capacity (rejecting far containers)\n");
        error_count++;
    }
    _container_data.erase(id); //TODO: do we need this? if rejected should stay on port
    return error_count;
}


void Crane::set_container_data(vector<unique_ptr<Container>> containers) {
    for(auto& container: containers) {
        _container_data[container -> getId()].emplace_back(std::move(container));
    }
}


pair<int, int> Crane::containerNotFoundError(const string &place) {
    std::ofstream file;
    file.open(_error_path, std::ios::out | std::ios::app);
    file << _sail_info << "Trying to get container that's not " << place << ". Instruction terminated.\n";
    file.close();
    return {0, 1};
}


int Crane::shouldReject(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route) {
    int errors = 0;
    if(_container_data.count(container -> getId()) > 0) errors++;
    if(plan.hasContainer(container -> getId())) errors++;
    if(!Reader::legalPortSymbol(container -> getDest())) errors++;
    if(!Reader::legalContainerId(container -> getId())) errors++;
    if(route.isLastStop()) errors++;
    if(plan.isFull()) errors++;
    if(!route.portInRoute(container -> getDest())) errors++;
    if(container-> getDest() == _port) errors++;
    return errors;
}


void Crane::writeInstructionError(const string &instruction, const string &id, bool executed) {
    std::ofstream file;
    file.open(_error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    string exec = executed ? "still" : "not";
    file << _sail_info << "ERROR: Algorithm is making a mistake with container " << id << ". " << instruction << " was " << exec << "executed.\n";
    file.close();
}


int Crane::isErrorLoad(unique_ptr<Container> &container, ShipPlan& plan, ShipRoute& route, Position pos, bool &execute) {
    int errors = 0;
    Position lowerFloor = Position(pos._floor - 1, pos._x, pos._y);
    bool isLegalLocation = plan.isLegalLocation(pos) && plan.isEmptyPosition(pos);
    bool cellBelowNull = pos._floor > 0 ? false : (plan.isLegalLocation(lowerFloor) && plan.isEmptyPosition(lowerFloor));
    if(!isLegalLocation || cellBelowNull) {
        writeInstructionError("Load", container -> getId(), false);
        execute = false;
        return 1;
    }
    if((errors = shouldReject(container, plan, route) > 0)) {
        writeInstructionError("Load", container -> getId(), true);
    }
    return errors;
}


int Crane::isErrorUnload(const string& id, ShipPlan &plan, Position pos, bool &execute) {
    int errors = 0;
    bool isLegalLocation = plan.isLegalLocation(pos);
    Position aboveFloor = Position(pos._floor + 1, pos._x, pos._y);
    bool cellAboveNull = pos._floor + 1 == plan.numberOfFloors() ? true : plan.isLegalFloor(aboveFloor) && !plan.isEmptyPosition(aboveFloor);

    if(!plan.hasContainer(id) || !isLegalLocation || !cellAboveNull) {
        writeInstructionError("Unload", id, false);
        execute = false;
        return 1;
    }

    if(plan.getIdAtPosition(pos) != id) {
        writeInstructionError("Unload", id, true);
        return 1;
    }

    return errors;
}

void Crane::writeRejectError(const string& id, const string& msg) {
    std::ofstream file;
    file.open(_error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    file << _sail_info << "ERROR: Algorithm is making a mistake when rejecting container " << id << ". FALSE REPORT:" <<msg;
    file.close();
}