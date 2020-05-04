#include "Crane.h"

#include <utility>

void Crane::set_operations(vector<Operation> operations) {
    _operations = std::move(operations);
}

pair<int,int> Crane::start(ShipPlan& plan, ShipRoute& route, vector<Operation> operations, const string &error_path, const string &sail_info) {
    set_operations(std::move(operations));
    int count = 0;
    for(Operation& op : _operations) {
        switch(op._operation) {
            case LOAD:
                count += load(op._container_id, op._position, plan, route, error_path, sail_info) ? 1 : 0;
                break;
            case UNLOAD:
                count += unload(op._container_id, op._position, plan, error_path, sail_info) ? 1 : 0;
                break;
            //reject assuming error code is in floor
            case REJECT:
                reject(op._container_id, op._position._floor, plan, route, error_path, sail_info);
                break;
        }
    }
    return {count, count}; //TODO
}

void Crane::end() { // TODO
    for (const auto& element : _container_data) {
        // if container dest != this port && ship not full => algorithm made mistake
        // delete element.second;
    }
    _container_data.clear();
}

bool Crane::load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route, const string &error_path, const string &sail_info) {
    unique_ptr<Container> container;
    if(_container_data.count(id) == 1) {
        container = std::move(_container_data[id]);
    }
    else {
        containerNotFoundError(error_path, sail_info, "at port");
        return false;
    }

    bool isLegalLocation = plan.isLegalLocation(pos);
    Position lowerFloor = Position(pos._floor - 1, pos._x, pos._y);
    bool cellBelowNull = pos._floor > 0 ? false : (plan.isLegalLocation(lowerFloor) && plan.isEmptyPosition(lowerFloor));
    if(!isLegalLocation || cellBelowNull || pos._floor != 0) {
        std::cout << "ERROR: Algorithm is making a mistake when loading " << container -> getId() << std::endl;
        return false;
    }
    if(!plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container))) { // TODO: what will happen to pointer if not insterted?
        std::cout << "ERROR: Algorithm is making a mistake when loading " << container -> getId() << std::endl;
        return false;
    }

    std::cout << "Loading container " << id << " to position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data.erase(container->getId());
    return true;
}

bool Crane::unload(string id, Position pos, ShipPlan& plan, const string &error_path, const string &sail_info) { // TODO: if making error, do we want the instruction to be made? what about _cargo_data?
    if(!plan.hasContainer(id)) {
        std::cout << "ERROR: Algorithm is making a mistake when trying to unload " << id << ". Instruction was not made." << std::endl;
        return false;
    }

    if((pos._floor + 1 < plan.numberOfFloors()) && !plan.getFloor(pos._floor + 1).isEmpty(pos._x, pos._y)) {
        std::cout << "ERROR: Algorithm is making a mistake when unloading " << id << ". Instruction was not made." << std::endl;
        return false;
    }
    pair<int, unique_ptr<Container>> removed = std::move(plan.getFloor(pos._floor).pop(pos._x, pos._y));
    if (removed.second == nullptr || removed.second -> getId() != id) {
        std::cout << "ERROR: Algorithm is making a mistake when unloading " << id << ". Instruction was made." << std::endl;
        return false;
    }

    std::cout << "Unloading container " << id << " from position: floor: " << pos._floor
              << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data[removed.second -> getId()] = std::move(removed.second);
    return true;
}

// return: true if rejecting, false if not
bool Crane::reject(string id, int errors, ShipPlan& plan, ShipRoute& route , const string &error_path, const string &sail_info) {
    unique_ptr<Container> container;
    if(_container_data.count(id) >= 1) {
        container = std::move(_container_data[id]);
    }
    else {
        containerNotFoundError(error_path, sail_info, "at port");
        return false;
    }
    if(errors == 0) { return false; } //TODO: algorithm didn't give a reject reason
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    if(errors & 10) {
        if(_container_data.count(id) == 1) { file << sail_info << "FALSE REPORT: containers at port: duplicate ID on port (ID rejected)\n"; }
    }
    if(errors & 11) {
        if(!plan.hasContainer(id)) { file << sail_info << "FALSE REPORT: containers at port: ID already on ship (ID rejected)\n"; }
    }
    if(errors & 12) {
        //TODO
        file << "containers at port: bad line format, missing or bad weight (ID rejected)\n";
    }
    if(errors & 13) {
        if(Reader::legalPortSymbol(container -> getDest())) { file << sail_info << "FALSE REOPRT: containers at port: bad line format, missing or bad port dest (ID rejected)\n"; }
    }
    if(errors & 14) {
        //TODO
        file << "containers at port: bad line format, ID cannot be read (ignored)\n";
    }
    if(errors & 15) {
        if(Reader::legalContainerId(id)) { file << sail_info << "FALSE REPORT: containers at port: illegal ID check ISO 6346 (ID rejected)\n"; }
    }
    if(errors & 16) {
        //TODO
        file << "containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n";
    }
    if(errors & 17) {
        if(!route.isLastStop()) { file << sail_info << "FALSE REPORT: containers at port: last port has waiting containers (ignored)\n"; }
    }
    if(errors & 18) {
        if(!plan.isFull()) { file << sail_info << "FALSE REPORT: containers at port: total containers amount exceeds ship capacity (rejecting far containers)\n"; }
    }
    //TODO
//    if(route.portInRoute(container -> getDest())) { file << sail_info << "FALSE REPORT: containers at port: destination of container isn't in route\n"; }
//    if(container-> getDest() != _port) { file << sail_info << "FALSE REPORT: containers at port: container's destination is the current port\n"; }

    file.close();
    _container_data.erase(id); //TODO: do we need this?
    //delete container; // TODO: what happens to the container here?
}


void Crane::set_container_data(map<string, unique_ptr <Container>> container_data) {
    _container_data = std::move(container_data); // TODO: notice the std::move ! where does it affect?
}

void Crane::containerNotFoundError(const string &error_path, const string &sail_info, const string &place) {
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app);
    file << sail_info << "Trying to get container that's not " << place << ". Instruction terminated.\n";
    file.close();
}

bool Crane::shouldReject(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route) {
    if(_container_data.count(container -> getId()) != 1) return true;
    if(plan.hasContainer(container -> getId())) return true;
    if(!Reader::legalPortSymbol(container -> getDest())) return true;
    if(!Reader::legalContainerId(container -> getId())) return true;
    if(route.isLastStop()) return true;
    if(plan.isFull()) return true;
    if(!route.portInRoute(container -> getDest())) return true;
    if(container-> getDest() == _port) return true;
    return false;
}
