#include "Crane.h"
#include "Simulation.h"

void Crane::set_container_data(vector<unique_ptr<Container>> containers) {
    for(auto& container: containers) {
        _container_data[container -> getId()].emplace_back(std::move(container));
    }
}

void Crane::set_operations(vector<Operation> operations) {
    _operations = std::move(operations);
}

void Crane::set_error_path(const string &error_path) {
    _error_path = error_path;
}

void Crane::set_sail_info(const string &sail_info) {
    _sail_info = sail_info;
}


void Crane::set_calculator(WeightBalanceCalculator &calculator) {
    _calculator = calculator;
}

int Crane::start(ShipPlan& plan, ShipRoute& route, WeightBalanceCalculator& calculator, vector<unique_ptr<Container>> containers, vector<Operation> operations, const string &error_path, const string &sail_info) {
    set_container_data(std::move(containers));
    set_operations(std::move(operations));
    set_error_path(error_path);
    set_sail_info(sail_info);
    set_calculator(calculator);
    _port = route.getCurrentPort();
    int sum_operations = 0;
    bool flag = false;
    for(Operation& op : _operations) {
        // TODO: is legal operation
        std::cout << "in port "<< _port<<" trying op: " << op._operation << " "<< op._container_id << " "<< op._position._floor<<op._position._x<<op._position._y << std::endl;
        switch(op._operation) {
            case LOAD:
                if(!load(op._container_id, op._position, plan, route)) { flag = true; }
                sum_operations++;
                break;
            case UNLOAD:
                if(!unload(op._container_id, op._position, plan)) { flag = true; }
                sum_operations++;
                break;
            case REJECT:
                if(!reject(op._container_id, plan, route)) { flag = true; }
                break;
            case MOVE:
                // TODO : check
                if(!unload(op._container_id, op._position, plan) || !load(op._container_id, op._move, plan, route)) {  flag = true; }
                sum_operations++;
                break;
        }
    }
    if(!end(plan, route)) { flag = true; }
    return flag ? FAILURE : sum_operations;
}

bool Crane::end(ShipPlan& plan, ShipRoute& route) {
    bool isLegal = checkLoadedTemporaryUnloaded() && checkWronglyUnloaded(plan, route) && checkShip(plan) && handleLastStop(plan, route);
    _container_data.clear();
    _temporary_unloaded.clear();
    _newly_loaded_dest.clear();
    return isLegal;
}

bool Crane::load(const string& id, Position pos, ShipPlan& plan, ShipRoute& route) {
    unique_ptr<Container> container;
    if(_container_data[id].empty()) {
        containerNotFoundError("at port");
        return false;
    }
    container = std::move(_container_data[id][0]);
    _container_data[id].erase(_container_data[id].begin());

    bool fatal = false;
    bool isError = isErrorLoad(container, plan, route, pos, fatal);
    if(fatal) { return false; }

    if(_calculator.tryOperation(LOAD, container -> getWeight(), pos._x, pos._y) != WeightBalanceCalculator::APPROVED) { return false; }

    if(_temporary_unloaded.find(container -> getId()) != _temporary_unloaded.end()) {
        _temporary_unloaded.erase(container -> getId());
    }
    else {
        _newly_loaded_dest.insert(container -> getDest());
    }

    plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container));
    std::cout << "Loading container " << id << " to position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data.erase(id);
    return !isError;
}

bool Crane::unload(const string& id, Position pos, ShipPlan& plan) {
    bool fatal = false;
    bool isError = isErrorUnload(id, plan, pos, fatal);
    if(fatal) { return false; }

    if(_calculator.tryOperation(UNLOAD, plan.getWeightById(id), pos._x, pos._y) != WeightBalanceCalculator::APPROVED) { return false; }

    unique_ptr<Container> removed = std::move(plan.getFloor(pos._floor).pop(pos._x, pos._y));
    if(removed -> getDest() != _port) {
        _temporary_unloaded.insert(removed -> getId());
    }
    std::cout << "Unloading container " << id << " from position: floor: " << pos._floor << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data[removed -> getId()].emplace_back(std::move(removed));
    return !isError;
}

bool Crane::reject(const string& id, ShipPlan& plan, ShipRoute& route) {
    unique_ptr<Container> container;
    if(!_container_data[id].empty()) {
        container = std::move(_container_data[id][0]);
        _container_data[id].erase(_container_data[id].begin());
    }
    else {
        containerNotFoundError("at port");
        return false;
    }
    if(shouldReject(container, plan, route) == 0) {
        writeInstructionError("Reject", container -> getId(), true);
        return false;
    }
    return true;
}

