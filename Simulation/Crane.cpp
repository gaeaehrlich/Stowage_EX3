#include "Crane.h"

template <typename T,typename U>
std::pair<T,U> operator+(const std::pair<T,U> & l,const std::pair<T,U> & r) {
    return {l.first+r.first,l.second+r.second};
}

void Crane::set_operations(vector<Operation> operations) {
    _operations = std::move(operations);
}

pair<int,int> Crane::start(ShipPlan& plan, ShipRoute& route, vector<Operation> operations, const string &error_path, const string &sail_info) {
    set_operations(std::move(operations));
    int succ = 0, err = 0;
    pair<int, int> result;
    for(Operation& op : _operations) {
        switch(op._operation) {
            case LOAD:
                result = load(op._container_id, op._position, plan, route, error_path, sail_info);
                succ += result.first;
                err += result.second;
                break;
            case UNLOAD:
                result = unload(op._container_id, op._position, plan, error_path, sail_info);
                succ += result.first;
                err += result.second;
                break;
            case REJECT: //reject assuming error code is in floor
                err += reject(op._container_id, op._position._floor, plan, route, error_path, sail_info);
                break;
        }
    }
    end(plan, error_path, sail_info);
    return {succ, err};
}

void Crane::end(ShipPlan& plan, const string &error_path, const string &sail_info) {
    std::ofstream file;
    for (const auto& element : _container_data) {
        if(element.second -> getDest() != _port && !plan.isFull()) {
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << sail_info << "ERROR: Algorithm is making a mistake with container " << element.second -> getId() << ". It was wrongly left at port " << _port << ".\n";
            file.close();
        }
    }
    _container_data.clear();
}

pair<int,int> Crane::load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route, const string &error_path, const string &sail_info) {
    unique_ptr<Container> container;
    if(_container_data.count(id) == 0) {
        return containerNotFoundError(error_path, sail_info, "at port");
    }
    container = std::move(_container_data[id]);

    bool execute = true;
    int errors = isErrorLoad(container, plan, route, pos, error_path, sail_info, execute);
    if(!execute) { return {0, errors}; }

    plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container));
    std::cout << "Loading container " << id << " to position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data.erase(id);
    return {1, errors};
}

pair<int,int> Crane::unload(string id, Position pos, ShipPlan& plan, const string &error_path, const string &sail_info) { // TODO: if making error, do we want the instruction to be made? what about _cargo_data?
    bool execute = true;
    int errors = isErrorUnload(id, plan, pos, error_path, sail_info, execute);
    if(!execute) { return {0, errors}; }

    pair<int, unique_ptr<Container>> removed = std::move(plan.getFloor(pos._floor).pop(pos._x, pos._y));
    std::cout << "Unloading container " << id << " from position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data[removed.second -> getId()] = std::move(removed.second);
    return {1, errors};
}

// return: true if rejecting, false if not
int Crane::reject(const string& id, int errors, ShipPlan& plan, ShipRoute& route , const string &error_path, const string &sail_info) {
    unique_ptr<Container> container;
    if(_container_data.count(id) >= 1) {
        container = std::move(_container_data[id]);
    }
    else {
        containerNotFoundError(error_path, sail_info, "at port");
        return false;
    }
    if(errors == 0) { return false; } //TODO: algorithm didn't give a reject reason
    int error_count = 0;
    std::ofstream file;
    if(errors & 10) {
        if(_container_data.count(id) == 1) {
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << sail_info << "FALSE REPORT: containers at port: duplicate ID on port (ID rejected)\n";
            file.close();
            error_count++;
        }
    }
    if(errors & 11) {
        if(!plan.hasContainer(id)) {
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << sail_info << "FALSE REPORT: containers at port: ID already on ship (ID rejected)\n";
            file.close();
            error_count++;
        }
    }
    if(errors & 12) {
        //TODO
        file << "containers at port: bad line format, missing or bad weight (ID rejected)\n";
        error_count++;
    }
    if(errors & 13) {
        if(Reader::legalPortSymbol(container -> getDest())) {
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << sail_info << "FALSE REOPRT: containers at port: bad line format, missing or bad port dest (ID rejected)\n";
            file.close();
            error_count++;
        }
    }
    if(errors & 14) {
        //TODO
        file << "containers at port: bad line format, ID cannot be read (ignored)\n";
        error_count++;
    }
    if(errors & 15) {
        if(Reader::legalContainerId(id)) {
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << sail_info << "FALSE REPORT: containers at port: illegal ID check ISO 6346 (ID rejected)\n";
            file.close();
            error_count++;
        }
    }
    if(errors & 16) {
        //TODO
        file << "containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n";
        error_count++;
    }
    if(errors & 17) {
        if(!route.isLastStop()) {
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << sail_info << "FALSE REPORT: containers at port: last port has waiting containers (ignored)\n";
            file.close();
            error_count++;
        }
    }
    if(errors & 18) {
        if(!plan.isFull()) {
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << sail_info << "FALSE REPORT: containers at port: total containers amount exceeds ship capacity (rejecting far containers)\n";
            file.close();
            error_count++;
        }
    }
    //TODO
//    if(route.portInRoute(container -> getDest())) { file << sail_info << "FALSE REPORT: containers at port: destination of container isn't in route\n"; }
//    if(container-> getDest() != _port) { file << sail_info << "FALSE REPORT: containers at port: container's destination is the current port\n"; }

    file.close();
    _container_data.erase(id); //TODO: do we need this?
    //delete container; // TODO: what happens to the container here?
    return error_count;
}


void Crane::set_container_data(map<string, unique_ptr <Container>> container_data) {
    _container_data = std::move(container_data); // TODO: notice the std::move ! where does it affect?
}


pair<int, int> Crane::containerNotFoundError(const string &error_path, const string &sail_info, const string &place) {
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app);
    file << sail_info << "Trying to get container that's not " << place << ". Instruction terminated.\n";
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


void Crane::writeInstructionError(const string &error_path, const string &sail_info, const string &instruction, const string &id, bool executed) {
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    string exec = executed ? "still" : "not";
    file << sail_info << "ERROR: Algorithm is making a mistake with container " << id << ". " << instruction << " was " << exec << "executed.\n";
    file.close();
}


int Crane::isErrorLoad(unique_ptr<Container> &container, ShipPlan& plan, ShipRoute& route, Position pos, const string &error_path, const string &sail_info, bool &execute) {
    int errors = 0;
    Position lowerFloor = Position(pos._floor - 1, pos._x, pos._y);
    bool isLegalLocation = plan.isLegalLocation(pos) && plan.isEmptyPosition(pos);
    bool cellBelowNull = pos._floor > 0 ? false : (plan.isLegalLocation(lowerFloor) && plan.isEmptyPosition(lowerFloor));
    if(!isLegalLocation || cellBelowNull) {
        writeInstructionError(error_path, sail_info, "Load", container -> getId(), false);
        execute = false;
        return 1;
    }
    if((errors = shouldReject(container, plan, route) > 0)) {
        writeInstructionError(error_path, sail_info, "Load", container -> getId(), true);
    }
    return errors;
}


int Crane::isErrorUnload(const string& id, ShipPlan &plan, Position pos, const string &error_path, const string &sail_info, bool &execute) {
    int errors = 0;
    bool isLegalLocation = plan.isLegalLocation(pos);
    Position aboveFloor = Position(pos._floor + 1, pos._x, pos._y);
    bool cellAboveNull = pos._floor + 1 == plan.numberOfFloors() ? true : plan.isLegalFloor(aboveFloor) && !plan.isEmptyPosition(aboveFloor);

    if(!plan.hasContainer(id) || !isLegalLocation || !cellAboveNull) {
        writeInstructionError(error_path, sail_info, "Unload", id, false);
        execute = false;
        return 1;
    }

    if(plan.getIdAtPosition(pos) != id) {
        writeInstructionError(error_path, sail_info, "Unload", id, true);
        return 1;
    }

    return errors;
}
