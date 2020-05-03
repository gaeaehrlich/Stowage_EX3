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
                count += load(op._container_id, op._position, plan, error_path, sail_info) ? 1 : 0;
                break;
            case UNLOAD:
                count += unload(op._container_id, op._position, plan, error_path, sail_info) ? 1 : 0;
                break;
            case REJECT: reject(op._container_id, op._position, error_path, sail_info);
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

bool Crane::load(const string& id, Position pos, ShipPlan& plan, const string &error_path, const string &sail_info) {
    unique_ptr<Container> container;
    if(_container_data.count(id) == 1) {
        container = std::move(_container_data[id]);
    }
    else {
        std::ofstream file;
        file.open(error_path, std::ios::out | std::ios::app);
        file << sail_info << "Trying to load container that's not waiting at port. Instruction terminated.\n";
        file.close();
        return false;
    }

    vector<pair<int, int>>  vec = pos._floor > 0 ? plan.getFloor(pos._floor - 1).getLegalLocations() : vector<pair<int, int>>();
    if(!Reader::legalContainerId(id) || !Reader::legalPortSymbol(container -> getDest())
       || plan.hasContainer(id) || ((pos._floor > 0) && std::count(vec.begin(), vec.end(), std::make_pair(pos._x, pos._y)) > 0
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

void Crane::reject(string id, Position pos, const string &error_path, const string &sail_info) {
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
