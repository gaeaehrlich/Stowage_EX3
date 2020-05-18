#include "NaiveAlgorithm.h"

NaiveAlgorithm::~NaiveAlgorithm() {
    _cargo_load.clear();
    _temporary_unloaded.clear();
    _invalid_travel = false;
    _status = 0;
}


void NaiveAlgorithm::finishedPort() {
    _cargo_load.clear();
    _temporary_unloaded.clear();
    _route.next();
}

int NaiveAlgorithm::readShipPlan(const string& full_path_and_file_name) {
    int readStatus = Reader::readShipPlan(full_path_and_file_name, _plan);
    if((readStatus & pow2(3)) || (readStatus & pow2(4))) _invalid_travel = true;
    _status |= readStatus;
    return readStatus;
}

int NaiveAlgorithm::readShipRoute(const string& full_path_and_file_name) {
    int readStatus = Reader::readShipRoute(full_path_and_file_name, _route);
    if((readStatus & pow2(7)) || (readStatus & pow2(8))) _invalid_travel = true;
    _status |= readStatus;
    return readStatus;
}

int NaiveAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator& calculator) {
    _calc = calculator;
    return 0;
}

int NaiveAlgorithm::getInstructionsForCargo(const string &input_path, const string &output_path) {
    std::ofstream file;
    file.open(output_path, std::ios::trunc);
    int port_status = 0;
    if(!_invalid_travel) {
        port_status = readCargoLoad(input_path);
        unloadInstructions(file);
        loadInstructions(file, _temporary_unloaded);
        sortCargoLoad();
        loadInstructions(file, _cargo_load);
    }
    file.close();
    finishedPort();
    return _status | port_status;
}


void NaiveAlgorithm::sortCargoLoad() {
    ShipRoute& route = _route;
    sort(_cargo_load.begin(), _cargo_load.end(), [route](const unique_ptr<Container>& a, const unique_ptr<Container>& b) {
        for(const string& port: route.getRoute()) {
            if(port == a -> getDest()) return true;
            if(port == b -> getDest()) return false;
        }
        return false;
    });
}


void NaiveAlgorithm::unloadInstructions(std::ofstream& file) {
    vector<Position> unload = _plan.findContainersToUnload(_route.getCurrentPort());
    for(const Position& pos: unload) {
        unloadContainersAbove(pos, file);
        if(_calc.tryOperation(UNLOAD, _plan.getWeightByPosition(pos), pos._x, pos._y) ==  WeightBalanceCalculator::APPROVED) {
            unique_ptr<Container> removed = std::move(_plan.getFloor(pos._floor).pop(pos._x, pos._y));
            file << instructionToString('U', removed -> getId(), pos);
        }
    }
}

void NaiveAlgorithm::loadInstructions(std::ofstream& file,  vector<unique_ptr<Container>>& list) {
    for(auto & container : list) {
        if(rejectingContainer(container)) {
            file << instructionToString('R', container -> getId(), Position(-1, -1, -1));
        }
        else {
            Position pos = findPosition(container);
            const string& id = container -> getId();
            file << instructionToString('L', id, pos);
            _plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container));
        }
    }
}

bool NaiveAlgorithm::rejectingContainer(unique_ptr<Container>& container) {
    if(countContainersOnPort(container ->getId()) > 1) { return true; }//containers at port: duplicate ID on port (ID rejected)
    if(_plan.hasContainer(container ->getId())) { return true; } //containers at port: ID already on ship (ID rejected)
    if(container -> getWeight() <= 0) { return true; } //containers at port: bad line format, missing or bad weight (ID rejected)
    if(!Reader::legalPortSymbol(container -> getDest())) { return true; } //containers at port: bad line format, missing or bad port dest (ID rejected)
    if(!_route.portInRoute(container -> getDest())) { return true; }
    if(container-> getDest() == _route.getCurrentPort()) { return true; }
    if(container -> getId().empty()) { return true; } // containers at port: bad line format, ID cannot be read (ignored)
    if(!Reader::legalContainerId(container -> getId())) { return true; } //containers at port: illegal ID check ISO 6346 (ID rejected)
    if(_route.isLastStop()) { return true; } //containers at port: last port has waiting containers (ignored)
    if(_plan.isFull()) { return true; } //containers at port: total containers amount exceeds ship capacity (rejecting far containers)
    return false;
}


void NaiveAlgorithm::unloadContainersAbove(Position pos, std::ofstream& file) {
    for(int i = _plan.numberOfFloors() - 1; i > pos._floor; i--) {
        if(!_plan.getFloor(i).isEmpty(pos._x, pos._y)) {
            if(_plan.getFloor(i).getContainerDest({pos._x, pos._y}) != _route.getCurrentPort()) {
                if(_calc.tryOperation(UNLOAD, _plan.getWeightByPosition(pos), pos._x, pos._y) ==  WeightBalanceCalculator::APPROVED) {
                    unique_ptr<Container> removed = _plan.getFloor(i).pop(pos._x, pos._y);
                    file << instructionToString('U', removed->getId(), Position(i, pos._x, pos._y));
                    _temporary_unloaded.emplace_back(std::move(removed));
                }
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

int NaiveAlgorithm::countContainersOnPort(const string& id) {
    int count = 0;
    for(auto& container: _cargo_load) {
        if(container != nullptr && container -> getId() == id) {
            count++;
        }
    }
    for(auto& container: _temporary_unloaded) {
        if(container != nullptr && container -> getId() == id) {
            count++;
        }
    }
    return count;
}


int NaiveAlgorithm::readCargoLoad(const string &input_path) {
    int readStatus =  Reader::readCargoLoad(input_path, _cargo_load);
    for(const auto& container: _cargo_load) {
        if(countContainersOnPort(container -> getId()) > 1) { readStatus |= pow2(10); }
        if(_plan.hasContainer(container -> getId())) { readStatus |= pow2(11); }
    }
    if(_route.isLastStop() && !_cargo_load.empty()) { readStatus |= pow2(17); }
    if(_plan.numberOfEmptyCells() + _plan.findContainersToUnload(_route.getCurrentPort()).size() < _cargo_load.size()) { readStatus |= pow2(18); }
    return readStatus;
}
