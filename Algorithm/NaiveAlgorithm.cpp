#include "NaiveAlgorithm.h"

void NaiveAlgorithm::clear() {
    _cargo_load.clear();
    _temporary_unloaded.clear();
    _invalid_travel = false;
    _status = 0;
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
    std::cout << "getting instructions" << std::endl;
    if(!_invalid_travel) {
        _status |= Reader::readCargoLoad(input_path, _cargo_load);
        if(_status & pow2(16)) {
            std::cout << "no cargo" << std::endl; //TODO ?
        }
        else {
            std::cout << "cargo load is: "; for(auto& c : _cargo_load) std::cout << c->getId() << " "; std::cout << std::endl;
            std::cout << "unloading" << std::endl;
            unloadInstructions(file);
            std::cout << "loading temp" << std::endl;
            _status |= loadInstructions(file, _temporary_unloaded);
            sortCargoLoad();
            std::cout << "loading cargo" << std::endl;
            _status |= loadInstructions(file, _cargo_load);
        }
    }
    file.close();
    clear();
    _route.next();
    return _status;
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
    vector<Position> unload = findContainersToUnload();
    for(const Position& pos: unload) {
        unloadContainersAbove(pos, file);
        if(_calc.tryOperation(UNLOAD, _plan.getWeightByPosition(pos), pos._x, pos._y) ==  WeightBalanceCalculator::APPROVED) {
            unique_ptr<Container> removed = std::move(_plan.getFloor(pos._floor).pop(pos._x, pos._y));
            file << instructionToString('U', removed -> getId(), pos);
        }
    }
}

int NaiveAlgorithm::loadInstructions(std::ofstream& file, vector<unique_ptr<Container>>& list) {
    int rejectReason = 0;
    for(auto & container : list) {
        rejectReason = rejectingContainer(container);
        if(rejectReason != 0) {
            file << instructionToString('R', container -> getId(), Position(rejectReason, rejectReason, rejectReason));
        }
        else {
            Position pos = findPosition(container);
            const string& id = container -> getId();
            file << instructionToString('L', id, pos);
            _plan.getFloor(pos._floor).insert(pos._x, pos._y, std::move(container));
        }
    }
    return rejectReason;
}

int NaiveAlgorithm::rejectingContainer(unique_ptr<Container>& container) {
    int reasons = 0;
    std::cout << "checking if should reject container: " << container->getId() << " going to " << container -> getDest()<< std::endl;
    if(countContainersOnPort(container ->getId()) > 1) { //containers at port: duplicate ID on port (ID rejected)
        if(!(reasons & pow2(10))) reasons |= pow2(10);
    }
    if(_plan.hasContainer(container ->getId())) { //containers at port: ID already on ship (ID rejected)
        reasons |= pow2(11);
    }
    if(container -> getWeight() <= 0) { //containers at port: bad line format, missing or bad weight (ID rejected)
        reasons |= pow2(12);
    }
    if(!Reader::legalPortSymbol(container -> getDest())
                         || !_route.portInRoute(container -> getDest())
                         || (container-> getDest() == _route.getCurrentPort())) { //containers at port: bad line format, missing or bad port dest (ID rejected)
        reasons |= pow2(13);
    }
    if(container -> getId().empty()) { // containers at port: bad line format, ID cannot be read (ignored)
        reasons |= pow2(14);
    }
    if(!Reader::legalContainerId(container ->getId())) { //containers at port: illegal ID check ISO 6346 (ID rejected)
        reasons |= pow2(15);
    }
    if(_route.isLastStop()) { //containers at port: last port has waiting containers (ignored)
        reasons |= pow2(17);
    }
    if(_plan.isFull()) { //containers at port: total containers amount exceeds ship capacity (rejecting far containers)
        reasons |= pow2(18);
    }
    return reasons;
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
