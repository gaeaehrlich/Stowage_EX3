#include "BaseAlgorithm.h"

BaseAlgorithm::~BaseAlgorithm() {
    _cargoLoad.clear();
    _temporaryUnloaded.clear();
    _invalidTravel = false;
    _status = 0;
}


void BaseAlgorithm::finishedPort() {
    _cargoLoad.clear();
    _temporaryUnloaded.clear();
    if(!_route.isLastStop()) _route.next();
}

int BaseAlgorithm::readShipPlan(const string& path) {
    int readStatus = Reader::readShipPlan(path, _plan);
    if((readStatus & pow2(3)) || (readStatus & pow2(4))) _invalidTravel = true;
    _status |= readStatus;
    return readStatus;
}

int BaseAlgorithm::readShipRoute(const string& path) {
    int readStatus = Reader::readShipRoute(path, _route);
    if((readStatus & pow2(7)) || (readStatus & pow2(8))) _invalidTravel = true;
    _status |= readStatus;
    return readStatus;
}

int BaseAlgorithm::setWeightBalanceCalculator(WeightBalanceCalculator& calculator) {
    _calc = calculator;
    return 0;
}

int BaseAlgorithm::getInstructionsForCargo(const string &inputPath, const string &outputPath) {
    std::ofstream file;
    file.open(outputPath, std::ios::trunc);
    int portStatus = 0;
    if(!_invalidTravel) {
        portStatus = readCargoLoad(inputPath);
        unloadInstructions(file);
        countSortCargo(_temporaryUnloaded, true);
        loadInstructions(file, _temporaryUnloaded); // todo: add _temporaryUnloaded to _cargoLoad in a SMART way and only then load
        rejectInstructions(file);
        sortCargoLoad();
        loadInstructions(file, _cargoLoad);
    }
    file.close();
    finishedPort();
    return _status | portStatus;
}

void BaseAlgorithm::rejectInstructions(std::ofstream &file) {
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

void BaseAlgorithm::removeDuplicates(std::ofstream &file) {
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

void BaseAlgorithm::sortCargoLoad() {
    countSortCargo(_cargoLoad);
    int numOfEmptyCells = _plan.numberOfEmptyCells();
    auto end = numOfEmptyCells >= _cargoLoad.size() ? _cargoLoad.end() : _cargoLoad.begin() + numOfEmptyCells;
    std::reverse(_cargoLoad.begin(), end);
}


void BaseAlgorithm::countSortCargo(vector<unique_ptr<Container>> &cargo, bool reverse) {
    int n = _route.getCurrentDistance();
    vector<vector<unique_ptr<Container>>> count(n + 1);
    for (auto & container : cargo) {
        int m = _plan.hasContainer(container->getId()) ? n : _route.portDistance(container->getDest());
        count[m].push_back(std::move(container));
    }
    cargo = vector<unique_ptr<Container>>();
    for (auto & vec : count) {
        for (auto & container : vec) {
            cargo.push_back(std::move(container));
        }
    }
    if (reverse) std::reverse(cargo.begin(), cargo.end());
}

void BaseAlgorithm::unloadInstructions(std::ofstream& file) {
    vector<Position> unload = _plan.findContainersToUnload(_route.getCurrentPort());
    buildUnloadPositions(unload);
    for(const Position& pos: unload) {
        unloadContainersAbove(pos, file);
        if(_calc.tryOperation(UNLOAD, _plan.getWeightByPosition(pos), pos._x, pos._y) ==  WeightBalanceCalculator::APPROVED) {
            unique_ptr<Container> removed = std::move(_plan.getFloor(pos._floor).pop(pos._x, pos._y));
            _countContainersToUnload[{pos._x, pos._y}] -= 1;
            file << instructionToString('U', removed -> getId(), pos);
        }
    }
    _countContainersToUnload.clear();
}

void BaseAlgorithm::loadInstructions(std::ofstream& file, vector<unique_ptr<Container>>& list) {
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

bool BaseAlgorithm::shouldRejectContainer(unique_ptr<Container>& container) {
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


void BaseAlgorithm::unloadContainersAbove(Position pos, std::ofstream& file) {
    for(int i = _plan.numberOfFloors() - 1; i > pos._floor; i--) {
        if(!_plan.getFloor(i).isEmpty(pos._x, pos._y)) {
            if(_plan.getFloor(i).getContainerDest({pos._x, pos._y}) != _route.getCurrentPort()) {
                unique_ptr<Container> removed = _plan.getFloor(i).pop(pos._x, pos._y);
                if(tryMoveFrom(removed, Position(i, pos._x, pos._y), file)) continue;
                if(_calc.tryOperation(UNLOAD, _plan.getWeightByPosition(pos), pos._x, pos._y) ==  WeightBalanceCalculator::APPROVED) {
                    file << instructionToString('U', removed->getId(), Position(i, pos._x, pos._y));
                    _temporaryUnloaded.emplace_back(std::move(removed));
                }
            }
        }
    }
}

string BaseAlgorithm::instructionToString(char instruction, const string& id, const Position pos, const Position move) {
    std::stringstream ss;
    string br = ", ";
    string newline = "\n";
    ss << instruction << br << id << br << pos._floor << br << pos._x << br << pos._y;
    if (instruction == 'M') ss << br << move._floor << br << move._x << br << move._y;
    ss << newline;
    return ss.str();
}

int BaseAlgorithm::countContainersOnPort(const string& id) {
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


int BaseAlgorithm::readCargoLoad(const string &input_path) {
    int readStatus =  Reader::readCargoLoad(input_path, _cargoLoad);
    for(const auto& container: _cargoLoad) {
        if(countContainersOnPort(container -> getId()) > 1) { readStatus |= pow2(10); }
        if(_plan.hasContainer(container -> getId())) { readStatus |= pow2(11); }
        if(!_route.portInRoute(container -> getDest())) { readStatus |= pow2(13); }
    }
    if(_route.isLastStop() && !_cargoLoad.empty()) {
        readStatus |= pow2(17);
        _cargoLoad.clear();
    }
    if(_plan.numberOfEmptyCells() + _plan.findContainersToUnload(_route.getCurrentPort()).size() < _cargoLoad.size()) { readStatus |= pow2(18); }
    return readStatus;
}

Position BaseAlgorithm::findPosition(const unique_ptr<Container> &container) {
    double diff = -1*(double)_route.getRoute().size(), weight = container -> getWeight();
    double newDist = _route.portDistance(container -> getDest());
    Position best;
    for(int i = 0; i < _plan.numberOfFloors() && diff != 0; ++i) {
        Floor& floor = _plan.getFloor(i);
        for(pair<int,int> location: floor.getLegalLocations()) {
            if(_plan.isLegalLoadPosition(Position(i, location.first, location.second)) &&
               _calc.tryOperation(LOAD,weight, location.first, location.second) == WeightBalanceCalculator::APPROVED) {
                double oldDist =  2.5; // if at the bottom, different calculation: found this number to be ideal
                if (_plan.isLegalLocation(Position(i - 1, location.first, location.second))) {
                    string port = _plan.getDestAtPosition(Position(i - 1, location.first, location.second));
                    oldDist = _route.portDistance(port);
                }
                if (_countContainersToUnload.find(location) != _countContainersToUnload.end()
                    && _countContainersToUnload[location]) { continue; } // in case of move operation
                if (diff < 0 ? diff < oldDist - newDist : (oldDist >= newDist && oldDist - newDist < diff)) {
                    diff = oldDist - newDist;
                    best = Position(i, location.first, location.second);
                    if (diff == 0) { break; } // ideal!
                }
            }
        }
    }
    return best;
}

void BaseAlgorithm::buildUnloadPositions(const vector<Position>& unload) {
    for (auto pos : unload) {
        if (_countContainersToUnload.find({pos._x, pos._y}) == _countContainersToUnload.end()) {
            _countContainersToUnload[{pos._x, pos._y}] =0;
        }
        _countContainersToUnload[{pos._x, pos._y}] += 1;
    }
}
