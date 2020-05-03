#include "Simulation.h"

string Simulation::getPath(const string& travel_dir, const string& search) {
    std::regex format("(.*)\\." + search);
    for(const auto & entry : fs::directory_iterator(travel_dir)) {
        if(std::regex_match(entry.path().stem().string(), format)) {
            return entry.path().string();
        }
    }
    return "";
}

bool Simulation::readShipPlan(const string& error_path, const string& travel_path, int travelNumber, unique_ptr<AbstractAlgorithm>& algorithm) {
    string plan_path = getPath(travel_path, "ship_plan");
    int errors = Reader::readShipPlan(plan_path, _plan);
    algorithm -> readShipPlan(plan_path);  // TODO: algorithm should be able to run even if files are bad
    return writeShipPlanErrors(error_path, errors);
}

bool Simulation::readShipRoute(const string& error_path, const string& travel_path, int travelNumber, unique_ptr<AbstractAlgorithm>& algorithm) {
    string route_path = getPath(travel_path, "route");
    int errors = Reader::readShipRoute(route_path, _route);
    algorithm -> readShipRoute(route_path); // TODO: algorithm should be able to run even if files are bad
    return writeShipRouteErrors(error_path, errors);
}


string Simulation::getCargoPath(const string &travel_dir, const string &port) {
    string pathName = travel_dir + SUBDIR + port + "_" + std::to_string(_route.getPortNumber()) + ".cargo_data";
    std::filesystem::path path = pathName;
    if(!std::filesystem::exists(path)) { // if cargo file doesn't exist, create an empty one
        std::ofstream cargo_file (pathName);
        cargo_file.close();
    }
    return pathName;
}

void Simulation::getInstructionForCargo(const string &cargoPath, const string &output_path, const string &error_path, vector<unique_ptr<Container>>& containersAtPort, unique_ptr<AbstractAlgorithm> &algorithm) {
    int errors = Reader::readCargoLoad(cargoPath, containersAtPort);
    algorithm -> getInstructionsForCargo(cargoPath, output_path);
    writeCargoErrors(error_path, errors); // void because to fatal errors
}

string Simulation::createPortOutputFile(const string &output_path, const string &port, int travelNumber) {
    string file = output_path + SUBDIR + port + "_" + std::to_string(travelNumber) + ".crane_instructions";
    std::ofstream outfile (file);
    outfile.close();
    return file;
}

pair<int, int> Simulation::sendInstructionsToCrane(const string &instructions_path, const string &error_path, const string &sail_ifo) {
    vector<Operation> instructions = Reader::getInstructionsVector(instructions_path);
    return _crane.start(_plan, _route, instructions, error_path, sail_ifo);
}

int Simulation::sail(unique_ptr<AbstractAlgorithm> &algorithm, const string &travel_path, int travelNumber, const string& output_path, const string& error_path) {
    string travel_dir = travel_path + SUBDIR + "Travel_" + std::to_string(travelNumber);
    for(const string& port: _route.getRoute()) {
        string cargoPath = getCargoPath(travel_dir, port);
        vector<unique_ptr<Container>> containersAtPort;
        string travel_output_path = createPortOutputFile(output_path, port, travelNumber);
        getInstructionForCargo(cargoPath, travel_output_path, error_path, containersAtPort, algorithm);
        string sail_info = "Algorithm: alg name, Travel: " + std::to_string(travelNumber) + " Port: " + port + "  : ";
        pair<int, int> results = sendInstructionsToCrane(travel_output_path, error_path, sail_info); // TODO alg name
        // TODO: check algorithm correctness
        _route.next();
    }
    return 0;
}


void Simulation::start(const string &travel_path, const string &algorithm_path, const string &output_path) {
    if(!checkDirectories(travel_path, algorithm_path, output_path)) { return; }
    vector<unique_ptr<AbstractAlgorithm>> algorithms; // TODO: init algorithms
    string error_path = output_path + SUBDIR + "simulation.errors";
    int numberTravels = Reader::getTravels(travel_path);
    for(auto& alg: algorithms) {
        for(int i = 0; i < numberTravels; i++) {
            if(!readShipPlan(error_path, travel_path, i, alg) || !readShipRoute(error_path, travel_path, i, alg)) { return; }
            string travel_output_path = output_path + SUBDIR + "<algorithm_name>" + "_Travel_" + std::to_string(i) + "_crane_instructions"; //TODO algorithm name
            fs::create_directory(travel_output_path);
            sail(alg, travel_path, i, travel_output_path, error_path);
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

bool Simulation::writeShipPlanErrors(const string &error_path, int errors) {
    if(errors == 0){
        return true;
    }
    bool fatal = false;
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    if(errors & 0) {
        file << " ship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n";
    }
    if(errors & 1) {
        file << "ship plan: a given position exceeds the X/Y ship limits (ignored)\n";
    }
    if(errors & 2) {
        file << "ship plan: bad line format after first line (ignored)\n";
    }
    if(errors & 3) {
        file << "ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n";
        fatal = true;
    }
    if(errors & 4) {
        file << "ship plan: travel error - duplicate x,y appearance with different data (cannot run this travel)";
        fatal = true;
    }
    file.close();
    return !fatal;
}

bool Simulation::writeShipRouteErrors(const string &error_path, int errors) {
    if(errors == 0){
        return true;
    }
    bool fatal = false;
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    if(errors & 5) {
        file << "travel route: a port appears twice or more consecutively (ignored)\n";
    }
    if(errors & 6) {
        file << "travel route: bad port symbol format (ignored)\n";
    }
    if(errors & 7) {
        file << "travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n";
        fatal = true;
    }
    if(errors & 8) {
        file << "travel route: travel error - file with only a single valid port (cannot run this travel)\n";
        fatal = true;
    }
    file.close();
    return !fatal;
}

bool Simulation::writeCargoErrors(const string &error_path, int errors) {
    if(errors == 0){
        return true;
    }
    bool fatal = false;
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    if(errors & 10) {
        file << "containers at port: duplicate ID on port (ID rejected)\n";
    }
    if(errors & 11) {
        file << "containers at port: ID already on ship (ID rejected)\n";
    }
    if(errors & 12) {
        file << "containers at port: bad line format, missing or bad weight (ID rejected)\n";
    }
    if(errors & 13) {
        file << "containers at port: bad line format, missing or bad port dest (ID rejected)\n";
    }
    if(errors & 14) {
        file << "containers at port: bad line format, ID cannot be read (ignored)\n";
    }
    if(errors & 15) {
        file << "containers at port: illegal ID check ISO 6346 (ID rejected)\n";
    }
    if(errors & 16) {
        file << "containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n";
    }
    if(errors & 17) {
        file << "containers at port: last port has waiting containers (ignored)\n";
    }
    if(errors & 18) {
        file << "containers at port: total containers amount exceeds ship capacity (rejecting far containers)\n";
    }
    file.close();
    return !fatal;
}
