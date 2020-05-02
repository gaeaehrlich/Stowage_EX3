#include "Crane.h"

void Crane::set_operations(vector<Operation> operations) {
    _operations = std::move(operations);
}

int Crane::start(ShipPlan& plan) {
    int count = 0;
    for(Operation& op : _operations) {
        switch(op._operation) {
            case LOAD:
                count += load(op._container_id, op._position, plan) ? 1 : 0;
                break;
            case UNLOAD:
                count += unload(op._container_id, op._position, plan) ? 1 : 0;
                break;
            case REJECT: reject(op._container_id, op._position);
                break;
        }
    }
    return count;
}

void Crane::end() { // TODO
    for (const auto& element : _container_data) {
        // if container dest != this port && ship not full => algorithm made mistake
        // delete element.second;
    }
    _container_data.clear();
}

bool Crane::load(const string& id, Position pos, ShipPlan& plan) {
    unique_ptr <Container> container;
    if(_container_data.count(id) == 1) {
        container = std::move(_container_data[id]);
    }
    else {
        //error
        //return
    }

    vector<pair<int, int>>  vec = pos._floor > 0 ? plan.getFloor(pos._floor - 1).getLegalLocations() : vector<pair<int, int>>();
    if(!Reader::legalContainerId(id) || !Reader::legalPortSymbol(container -> getDest())
       || plan.hasContainer(id) || ((pos._floor > 0) && std::count(vec.begin(), vec.end(), make_pair(pos._x, pos._y)) > 0
           && plan.getFloor(pos._floor - 1).isEmpty(pos._x, pos._y))) {
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

bool Crane::unload(string id, Position pos, ShipPlan& plan) {
    unique_ptr <Container> container;
    if(plan.hasContainer(id)) {
        container = std::move(plan.getFloor(pos._floor).getContainer({pos._x, pos._y}));
        if (container == nullptr || container -> getId() != id) {
            //error
            //return
        }
    }
    else {
        //error
        //return
    }
    if((pos._floor + 1 < plan.numberOfFloors()) && !plan.getFloor(pos._floor + 1).isEmpty(pos._x, pos._y)) {
        std::cout << "ERROR: Algorithm is making a mistake when unloading " << container -> getId() << std::endl;
        return false;
    }
    unique_ptr <Container> removed = std::move(plan.getFloor(pos._floor).pop(pos._x, pos._y));
    if (removed == nullptr) {
        std::cout << "ERROR: Algorithm is making a mistake when unloading " << container -> getId() << std::endl;
        return false;
    }

    std::cout << "Unloading container " << container -> getId() << " from position: floor: " << pos._floor
              << ", x: " << pos._x << ", y: " << pos._y << std::endl;
    _container_data[removed -> getId()] = std::move(removed);
    return true;
}

void Crane::reject(string id, Position pos) {
    string reason;
    switch(pos._floor) {
        case SPACE: reason = "Not enough space on ship";
            break;
        case ID: reason = "Wrong ID format for container";
            break;
        case DEST: reason = "Destination of container isn't in route";
            break;
        case SYMBOL: reason = "Wrong symbol format for the destination port";
            break;
        case CURR: reason = "This container's destination is the current port";
            break;
        case EXISTS: reason = "This container already exists on the ship";
            break;
        default:
            reason = "WARNING: Algorithm did not give a reason for rejecting";
    }
    std::cout << "Rejecting container " << id << ": " << reason << std::endl;
    _container_data.erase(id);
    //delete container; // TODO: what happens to the container here?
}


void Crane::set_container_data(map<string, unique_ptr <Container>> container_data) {
    _container_data = std::move(container_data); // TODO: notice the std::move ! where does it affect?
}