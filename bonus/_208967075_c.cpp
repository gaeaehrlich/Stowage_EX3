#include "_208967075_c.h"
REGISTER_ALGORITHM(_208967075_c)

_208967075_c::~_208967075_c() {
    _cargoLoad.clear();
}


void _208967075_c::finishedPort() {
    _cargoLoad.clear();
    _route.next();
}

int _208967075_c::readShipPlan(const string& path) {
    Reader::readShipPlan(path, _plan);
    return 0;
}

int _208967075_c::readShipRoute(const string& path) {
    Reader::readShipRoute(path, _route);
    return 0;
}

int _208967075_c::readCargoLoad(const string &path) {
    Reader::readCargoLoad(path, _cargoLoad);
    return 0;
}

int _208967075_c::setWeightBalanceCalculator(WeightBalanceCalculator& calculator) {
    _calc = calculator;
    return 0;
}

int _208967075_c::getInstructionsForCargo(const string &inputPath, const string &outputPath) {
    std::ofstream file;
    file.open(outputPath, std::ios::trunc);
    readCargoLoad(inputPath);
    unloadInstructions(file);
    loadInstructions(file, _cargoLoad);
    file.close();
    finishedPort();
    return 0;
}

void _208967075_c::unloadInstructions(std::ofstream& file) {
    vector<Position> unload = _plan.findContainersToUnload(_route.getCurrentPort());
    for(const Position& pos: unload) {
        if(_plan.getFloor(pos._floor).getContainerID(pos._x, pos._y).at(0) != 'C') {
            unloadContainersAbove(pos, file);
            if(_calc.tryOperation(UNLOAD, _plan.getWeightByPosition(pos), pos._x, pos._y) ==  WeightBalanceCalculator::APPROVED) {
                unique_ptr<Container> removed = std::move(_plan.getFloor(pos._floor).pop(pos._x, pos._y));
                file << instructionToString('U', removed -> getId(), pos);
            }
        }
    }
}

void _208967075_c::loadInstructions(std::ofstream& file,  vector<unique_ptr<Container>>& list) {
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

bool _208967075_c::rejectingContainer(unique_ptr<Container>& container) {
    if(_plan.hasContainer(container -> getId())) { return true; } //containers at port: ID already on ship (ID rejected)
    if(_plan.isFull()) { return true; } //containers at port: total containers amount exceeds ship capacity (rejecting far containers)
    return false;
    // containers at port: duplicate ID on port (ID rejected)
    // containers at port: bad line format, missing or bad weight (ID rejected)
    // containers at port: bad line format, missing or bad port dest (ID rejected)
    // containers at port: container's destination is in route
    // containers at port: container's destination is current port
    // containers at port: bad line format, ID cannot be read (ignored)
    // containers at port: illegal ID check ISO 6346 (ID rejected)
    // containers at port: last port has waiting containers (ignored)

}


void _208967075_c::unloadContainersAbove(Position pos, std::ofstream& file) {
    for(int i = _plan.numberOfFloors() - 1; i > pos._floor; i--) {
        if(!_plan.getFloor(i).isEmpty(pos._x, pos._y)) {
            if(_plan.getFloor(i).getContainerDest({pos._x, pos._y}) != _route.getCurrentPort()) {
                if(_calc.tryOperation(UNLOAD, _plan.getWeightByPosition(pos), pos._x, pos._y) ==  WeightBalanceCalculator::APPROVED) {
                    unique_ptr<Container> removed = _plan.getFloor(i).pop(pos._x, pos._y);
                    file << instructionToString('U', removed->getId(), Position(i, pos._x, pos._y));
                    _cargoLoad.emplace_back(std::move(removed));
                }
            }
        }
    }
}

string _208967075_c::instructionToString(char instruction, const string& id, Position pos) {
    std::stringstream ss;
    string br = ", ";
    string newline = "\n";
    ss << instruction << br << id << br << pos._floor << br << pos._x << br << pos._y << newline;
    return ss.str();
}


Position _208967075_c::findPosition(const unique_ptr<Container>& container) {
    int weight = container -> getWeight();
    for(int i = 0; i < _plan.numberOfFloors(); ++i) {
        Floor& floor = _plan.getFloor(i);
        for(pair<int,int> location: floor.getLegalLocations()) {
            if(floor.isEmpty(location) && _calc.tryOperation(LOAD, weight, location.first, location.second) == WeightBalanceCalculator::APPROVED) {
                return Position(i, location.first, location.second);
            }
        }
    }
    return Position();
}
