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

int Crane::start(ShipPlan& plan, ShipRoute& route, vector<unique_ptr<Container>> containers, vector<Operation> operations, const string &error_path, const string &sail_info) {
    set_container_data(std::move(containers));
    set_operations(std::move(operations));
    set_error_path(error_path);
    set_sail_info(sail_info);
    int sum_operations = 0;
    for(Operation& op : _operations) {
        switch(op._operation) {
            case LOAD:
                if(!load(op._container_id, op._position, plan, route)) { return -1; }
                sum_operations++;
                break;
            case UNLOAD:
                if(!unload(op._container_id, op._position, plan)) { return -1; }
                sum_operations++;
                break;
            case REJECT:
                if(!reject(op._container_id, plan, route)) { return -1; }
                break;
        }
    }
    end(plan);
    return sum_operations;
}

void Crane::end(ShipPlan& plan) {
    // השארה על הרציף של מכולה עם יעד קרוב כאשר נטענה לאוניה בנמל זה מכולה עם יעד רחוק יותר, או השארה על הרציף של מכולה שכבר היתה על האוניה והיעד שלה אינו נמל זה
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

bool Crane::load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route) {
    unique_ptr<Container> container;
    if(_container_data.count(id) == 0) {
        containerNotFoundError("at port");
        return false;
    }
    container = std::move(_container_data[id][0]);
    _container_data[id].erase(_container_data[id].begin());

    if(isErrorLoad(container, plan, route, pos)) { return false; }

    plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container));
    std::cout << "Loading container " << id << " to position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data.erase(id);
    return true;
}

bool Crane::unload(const string& id, Position pos, ShipPlan& plan) { // TODO: if making error, do we want the instruction to be made? what about _cargo_data?
    if(isErrorUnload(id, plan, pos)) { return false; }

    pair<int, unique_ptr<Container>> removed = std::move(plan.getFloor(pos._floor).pop(pos._x, pos._y));
    std::cout << "Unloading container " << id << " from position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data[removed.second -> getId()].emplace_back(std::move(removed.second));
    return true;
}

bool Crane::reject(const string& id, ShipPlan& plan, ShipRoute& route) {
    unique_ptr<Container> container;
    if(_container_data.count(id) >= 1) {
        container = std::move(_container_data[id][0]);
        _container_data[id].erase(_container_data[id].begin());
    }
    else {
        containerNotFoundError("at port");
        return false;
    }
    int error_count = shouldReject(container, plan, route);
    if(error_count == 0) {
        writeInstructionError("Reject", container -> getId(), true);
    }
    return error_count > 0;
}


void Crane::set_container_data(vector<unique_ptr<Container>> containers) {
    for(auto& container: containers) {
        _container_data[container -> getId()].emplace_back(std::move(container));
    }
}


void Crane::containerNotFoundError(const string &place) {
    std::ofstream file;
    file.open(_error_path, std::ios::out | std::ios::app);
    file << _sail_info << "Trying to get container that's not " << place << ". Instruction terminated.\n";
    file.close();
}


int Crane::shouldReject(unique_ptr<Container>& container, ShipPlan& plan, ShipRoute& route) {
    int errors = 0;
    if(_container_data.count(container -> getId()) > 0) errors++;
    if(plan.hasContainer(container -> getId())) errors++;
    if(container -> getWeight() < 0) errors++;
    if(!Reader::legalPortSymbol(container -> getDest())) errors++;
    if(!Reader::legalContainerId(container -> getId())) errors++;
    if(container -> getId().empty()) errors++;
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


bool Crane::isErrorLoad(unique_ptr<Container> &container, ShipPlan& plan, ShipRoute& route, Position pos) {
    Position lowerFloor = Position(pos._floor - 1, pos._x, pos._y);
    bool isLegalLocation = plan.isLegalLocation(pos) && plan.isEmptyPosition(pos);
    bool cellBelowNull = pos._floor > 0 ? false : (plan.isLegalLocation(lowerFloor) && plan.isEmptyPosition(lowerFloor));
    if(!isLegalLocation || cellBelowNull) {
        writeInstructionError("Load", container -> getId(), false);
        return true;
    }
    if(shouldReject(container, plan, route) > 0) {
        writeInstructionError("Load", container -> getId(), true);
    }
    return false;
}


bool Crane::isErrorUnload(const string& id, ShipPlan &plan, Position pos) {
    bool isLegalLocation = plan.isLegalLocation(pos);
    Position aboveFloor = Position(pos._floor + 1, pos._x, pos._y);
    bool cellAboveNull = pos._floor + 1 == plan.numberOfFloors() ? true : plan.isLegalFloor(aboveFloor) && !plan.isEmptyPosition(aboveFloor);

    if(!plan.hasContainer(id) || !isLegalLocation || !cellAboveNull) {
        writeInstructionError("Unload", id, false);
        return true;
    }

    if(plan.getIdAtPosition(pos) != id) {
        writeInstructionError("Unload", id, true);
    }
    return false;
}