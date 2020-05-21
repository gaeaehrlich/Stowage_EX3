#include "NaiveAlgorithm.h"

NaiveAlgorithm::~NaiveAlgorithm() {
    _cargoLoad.clear();
    _temporaryUnloaded.clear();
    _invalidTravel = false;
    _status = 0;
}


void NaiveAlgorithm::finishedPort() {
    _cargoLoad.clear();
    _temporaryUnloaded.clear();
    if(!_route.isLastStop()) _route.next();
}

int NaiveAlgorithm::readShipPlan(const string& path) {
    int readStatus = Reader::readShipPlan(path, _plan);
    if((readStatus & pow2(3)) || (readStatus & pow2(4))) _invalidTravel = true;
    _status |= readStatus;
    return readStatus;
}

int NaiveAlgorithm::readShipRoute(const string& path) {
    int readStatus = Reader::readShipRoute(path, _route);
    if((readStatus & pow2(7)) || (readStatus & pow2(8))) _invalidTravel = true;
    _status |= readStatus;
    return readStatus;
}

int NaiveAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator& calculator) {
    _calc = calculator;
    return 0;
}

int NaiveAlgorithm::getInstructionsForCargo(const string &inputPath, const string &outputPath) {
    std::ofstream file;
    file.open(outputPath, std::ios::trunc);
    int portStatus = 0;
    if(!_invalidTravel) {
        portStatus = readCargoLoad(inputPath);
        unloadInstructions(file);
        loadInstructions(file, _temporaryUnloaded);
        rejectInstructions(file);
        sortCargoLoad();
        loadInstructions(file, _cargoLoad);
    }
    file.close();
    finishedPort();
    return _status | portStatus;
}

void NaiveAlgorithm::rejectInstructions(std::ofstream &file) {
    removeDuplicates(file);
    for(auto container = _cargoLoad.begin(); container != _cargoLoad.end();) {
        if(shouldRejectContainer(*container)) {
            file << instructionToString('R', (*container) -> getId(), Position());
            _cargoLoad.erase(container);
        }
        else {
            ++container;
        }
    }
}

void NaiveAlgorithm::removeDuplicates(std::ofstream &file) {
    std::unordered_set<string> idsOnPort;
    for(auto container = _cargoLoad.begin(); container != _cargoLoad.end();) {
        if(idsOnPort.find((*container) -> getId()) != idsOnPort.end()) {
            file << instructionToString('R', (*container) -> getId(), Position());
            _cargoLoad.erase(container);
        }
        else {
            idsOnPort.insert((*container) -> getId());
            ++container;
        }
    }
}

void NaiveAlgorithm::sortCargoLoad() {
    ShipRoute& route = _route;
    sort(_cargoLoad.begin(), _cargoLoad.end(), [route](const unique_ptr<Container>& a, const unique_ptr<Container>& b) {
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
        if(_plan.isFull() || _plan.hasContainer(container -> getId())) { // all other reject reasons were already handled
            file << instructionToString('R', container -> getId(), Position());
        }
        else {
            Position pos = findPosition(container);
            const string& id = container -> getId();
            file << instructionToString('L', id, pos);
            _plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container));
        }
    }
}

bool NaiveAlgorithm::shouldRejectContainer(unique_ptr<Container>& container) {
    //if(_plan.hasContainer(container -> getId())) { return true; } //containers at port: ID already on ship (ID rejected)
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
                    _temporaryUnloaded.emplace_back(std::move(removed));
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
    for(auto& container: _cargoLoad) {
        if(container != nullptr && container -> getId() == id) {
            count++;
        }
    }
    for(auto& container: _temporaryUnloaded) {
        if(container != nullptr && container -> getId() == id) {
            count++;
        }
    }
    return count;
}


int NaiveAlgorithm::readCargoLoad(const string &input_path) {
    int readStatus =  Reader::readCargoLoad(input_path, _cargoLoad);
    for(const auto& container: _cargoLoad) {
        if(countContainersOnPort(container -> getId()) > 1) { readStatus |= pow2(10); }
        if(_plan.hasContainer(container -> getId())) { readStatus |= pow2(11); }
    }
    if(_route.isLastStop() && !_cargoLoad.empty()) {
        readStatus |= pow2(17);
        _cargoLoad.clear();
    }
    if(_plan.numberOfEmptyCells() + _plan.findContainersToUnload(_route.getCurrentPort()).size() < _cargoLoad.size()) { readStatus |= pow2(18); }
    return readStatus;
}
