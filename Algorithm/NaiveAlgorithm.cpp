#include "NaiveAlgorithm.h"

int NaiveAlgorithm::readShipPlan(const string& full_path_and_file_name) { // TODO
    return Reader::readShipPlan(full_path_and_file_name, _plan);
}

int NaiveAlgorithm::readShipRoute(const string& full_path_and_file_name) {
    return Reader::readShipRoute(full_path_and_file_name, _route);
}

int NaiveAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator& calculator) {
    return 0;
}

int NaiveAlgorithm::getInstructionsForCargo(const string &input_path, const string &output_path) {
    if(!_route.isLastStop()) {
        Reader::readCargoLoad(input_path, _cargo_load);
    }
    unloadInstructions(output_path);
    loadInstructions(output_path, _temporary_unloaded);
    sortCargoLoad();
    loadInstructions(output_path, _cargo_load);
    //return _calc.balance(vector<Operation>()) == APPROVED; // TODO
    return 0;
}


bool NaiveAlgorithm::cmpContainers(const unique_ptr<Container>& a, const unique_ptr<Container>& b) {
    for(const string& port: _route.getRoute()) {
        if(port == a -> getDest()) return true;
        if(port == b -> getDest()) return false;
    }
    return false;
}

void NaiveAlgorithm::sortCargoLoad() { // TODO: new sort! check!
    ShipRoute& route = _route; // TODO: should this be by reference?
    sort(_cargo_load.begin(), _cargo_load.end(), [route](const unique_ptr<Container>& a, const unique_ptr<Container>& b) {
        for(const string& port: route.getRoute()) {
            if(port == a -> getDest()) return true;
            if(port == b -> getDest()) return false;
        }
        return false;
    });
}


void NaiveAlgorithm::unloadInstructions(const string &output_path) {
    std::ofstream file;
    file.open (output_path, std::ios::trunc);
    vector<Position> unload = findContainersToUnload();
    for(const Position& pos: unload) {
        unloadContainersAbove(pos, file);
        unique_ptr<Container> removed = std::move(_plan.getFloor(pos._floor).pop(pos._x, pos._y));
        file << instructionToString('U', removed -> getId(), pos);
        // delete container;
    }
    file.close();
}

void NaiveAlgorithm::loadInstructions(const string &output_path, vector<unique_ptr<Container>>& list) {
    std::ofstream file;
    file.open (output_path, std::ios::app);
    for(int i = 0; i < list.size(); i++) {
        int rejectReason = rejectingContainer(list[i] -> getId(), list[i] -> getDest());
        if(rejectReason < 0) {
            file << instructionToString('R', list[i] -> getId(), Position(rejectReason, rejectReason, rejectReason));
            //delete container;
        }
        else {
            Position pos = findPosition();
            file << instructionToString('L', list[i] -> getId(), pos);
            _plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(list[i]));
        }
    }
    //list.clear(); // TODO: do we really want to clear? we want to check correctness
    file.close();
}

Position NaiveAlgorithm::findPosition() {
    for(int i = 0; i < _plan.numberOfFloors(); ++i) {
        Floor& floor = _plan.getFloor(i);
        for(pair<int,int> location: floor.getLegalLocations()) {
            if(floor.isEmpty(location)) {
                return Position(i, location.first, location.second); // TODO: is retrun okay?
            }
        }
    }
    return Position(-1, -1, -1);
}

int NaiveAlgorithm::rejectingContainer(const string& id, string dest) {
    if (!Reader::legalContainerId(id)) {
        return ID;
    }
    else if (!Reader::legalPortSymbol(dest)) {
        return SYMBOL;
    }
    else if (dest == _route.getCurrentPort()) {
        return CURR;
    }
    else if (!_route.portInRoute(dest)) {
        return DEST;
    }
    else if (_plan.hasContainer(id)) {
        return EXISTS;
    }
    else if (_plan.isFull()) {
        return SPACE;
    }
    return 1;
}

vector<Position> NaiveAlgorithm::findContainersToUnload() {
    vector<Position> unload;
    for(int i = _plan.numberOfFloors() - 1; i >= 0 ; i--) {
        Floor& floor = _plan.getFloor(i);
        for(pair<int,int> location: floor.getLegalLocations()) {
            if(!floor.isEmpty(location) && floor.getContainerDest(location) == _route.getCurrentPort()) {
                unload.emplace_back(Position(i, location.first, location.second));
            }
        }
    }
    return unload;
}

void NaiveAlgorithm::unloadContainersAbove(Position pos, std::ofstream& file) {
    for(int i = _plan.numberOfFloors() - 1; i > pos._floor; i--) {
        if(!_plan.getFloor(i).isEmpty(pos._x, pos._y)) {
            if(_plan.getFloor(i).getContainerDest({pos._x, pos._y}) != _route.getCurrentPort()) {
                unique_ptr<Container> removed = _plan.getFloor(i).pop(pos._x, pos._y);
                file << instructionToString('U', removed -> getId(), Position(i, pos._x, pos._y));
                _temporary_unloaded.emplace_back(std::move(removed));
            }
        }
    }
}

string NaiveAlgorithm::instructionToString(char instruction, const string& id, Position pos) {
    std::stringstream ss;
    string br = ", ";
    string newline = "\n";
    ss << instruction << br << id << br << pos._floor << br << pos._x << br << pos._y << newline;
    return ss.str();
}
