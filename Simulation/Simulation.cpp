#include "Simulation.h"

bool Simulation::readShipPlan(const string& error_path, const string& travel_path, const string& travel, unique_ptr<AbstractAlgorithm>& algorithm) {
    string plan_path = getPath(travel_path, "ship_plan");
    int errors = Reader::readShipPlan(plan_path, _plan);
    algorithm -> readShipPlan(plan_path); // TODO: ignoring if alg read failed
    return writeShipPlanErrors(error_path, errors, travel);
}

bool Simulation::readShipRoute(const string& error_path, const string& travel_path, const string& travel, unique_ptr<AbstractAlgorithm>& algorithm) {
    string route_path = getPath(travel_path, "route");
    int errors = Reader::readShipRoute(route_path, _route);
    algorithm -> readShipRoute(route_path); // TODO: ignoring if alg read failed
    return writeShipRouteErrors(error_path, errors, travel);
}

void Simulation::getInstructionForCargo(const string &cargoPath, const string &output_path, const string &error_path, vector<unique_ptr<Container>>& containersAtPort, unique_ptr<AbstractAlgorithm> &algorithm) {
    int errors = Reader::readCargoLoad(cargoPath, containersAtPort);
    algorithm -> getInstructionsForCargo(cargoPath, output_path);
    writeCargoErrors(error_path, errors); // void because no fatal errors
}

int Simulation::sendInstructionsToCrane(vector<unique_ptr<Container>> containers, const string &instructions_path, const string &error_path, const string &sail_info) {
    vector<Operation> instructions = Reader::getInstructionsVector(instructions_path);
    return _crane.start(_plan, _route, std::move(containers), instructions, error_path, sail_info);
}

int Simulation::sail(unique_ptr<AbstractAlgorithm> &algorithm, const string &travel_path, const string& travel_name, const string& output_path, const string& error_path) {
    int results = 0;
    string travel_output_path = createTravelOutputFolder(output_path, "<alg name>", travel_name); // TODO: alg name
    for(const string& port: _route.getRoute()) {
        string cargoPath = getCargoPath(travel_path, port);
        vector<unique_ptr<Container>> containersAtPort;
        string port_output_path = createPortOutputFile(travel_output_path, port);
        getInstructionForCargo(cargoPath, port_output_path, error_path, containersAtPort, algorithm);
        string sail_info = "Algorithm: <alg name>, Travel: " + travel_name + " Port: " + port + "  : \n";
        results += sendInstructionsToCrane(std::move(containersAtPort), port_output_path, error_path, sail_info); // TODO alg name
        // TODO: "finished" at error file?
        _route.next();
    }
    return results;
}


void Simulation::start(const string &travel_path, const string &algorithm_path, const string &output_path) {
    if(!checkDirectories(travel_path, algorithm_path, output_path)) { return; }
    string error_path = output_path + SUBDIR + "simulation.errors";
    string results_path = output_path + SUBDIR + "simulation.results";
    vector<string> travels = Reader::getTravels(travel_path);
    map<string, vector<int>> alg_results;
    for(int i = 0; i < travels.size(); i++) {
        vector<unique_ptr<AbstractAlgorithm>> algorithms; // TODO: init algorithms
        for(auto& alg: algorithms) {
            string curr_travel_path = travel_path + SUBDIR + travels[i];
            if(!readShipPlan(error_path, curr_travel_path, travels[i], alg) || !readShipRoute(error_path, curr_travel_path, travels[i], alg)) { continue; }
            alg_results["<algorithm_name>"].emplace_back(sail(alg, curr_travel_path, travels[i], output_path, error_path)); // TODO: alg name
        }
    }
    writeResults(results_path, alg_results, travels);
}
