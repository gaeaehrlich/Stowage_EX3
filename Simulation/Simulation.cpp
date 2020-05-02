#include "Simulation.h"


bool Simulation::readShipPlan(const string& travel_path, int travelNumber, unique_ptr<AbstractAlgorithm>& algorithm) {
    string travel_dir = travel_path + "\\Travel_" + std::to_string(travelNumber);
    string plan_path = Reader::getPath(travel_dir, "Plan");
    if(plan_path.empty()) {
        std::cout << "Content of directory " << travel_dir <<" is missing the ship plan file" << std::endl;
        return false;
    }
    else {
        Reader::readShipPlan(plan_path, _plan);
        algorithm -> readShipPlan(plan_path);
        return true;
    }
}
bool Simulation::readShipRoute(const string& travel_path, int travelNumber, unique_ptr<AbstractAlgorithm>& algorithm) {
    string travel_dir = travel_path + "\\Travel_" + std::to_string(travelNumber);
    string route_path = Reader::getPath(travel_dir, "Plan");
    if(route_path.empty()) {
        std::cout << "Content of directory " << travel_dir <<" is missing the ship route file" << std::endl;
        return false;
    }
    else {
        Reader::readShipRoute(route_path, _route);
        algorithm -> readShipRoute(route_path);
        return true;
    }
}


int Simulation::sail(unique_ptr<AbstractAlgorithm> &algorithm, const string &travel_path, int travelNumber, const string& output_path) {
    string travel_dir = travel_path + "\\Travel_" + std::to_string(travelNumber);
    for(const string& port: _route.getRoute()) {
        string cargoPath = travel_dir + "\\" + port + "_" + std::to_string(_route.getPortNumber()) + ".cargo_data";
        algorithm -> getInstructionsForCargo(cargoPath, output_path);
        _route.next();
    }
    return 0;
}


void Simulation::start(const string &travel_path, const string &algorithm_path, const string &output_path) {
    if(!checkDirectories(travel_path, algorithm_path, output_path)) { return; }
    // TODO: init algorithms
    vector<unique_ptr<AbstractAlgorithm>> algorithms;
    int numberTravels = Reader::getTravels(travel_path);
    for(auto& alg: algorithms) {
        for(int i = 0; i < numberTravels; i++) {
            if(!readShipPlan(travel_path, i, alg) || !readShipRoute(travel_path, i, alg)) { return; }
            sail(alg, travel_path, i, output_path);
        }
    }
}

bool Simulation::checkDirectories(const string &travel_path, const string &algorithm_path, const string &output_path) {
    bool travel = Reader::checkDirPath(travel_path);
    bool algorithm = Reader::checkDirPath(algorithm_path);
    bool output = Reader::checkDirPath(output_path);
    if(!travel) {
        std::cout << "The path for the travels is incorrect: " << travel_path << std::endl;
    }
    if(!algorithm) {
        std::cout << "The path for the algorithms is incorrect: " << algorithm_path << std::endl;
    }
    if(!output) {
        std::cout << "The path for the output is incorrect: " << output_path << std::endl;
    }
    return travel && algorithm && output;
}
