#include "Simulation.h"

bool Simulation::readShipPlan(const string& error_path, const string& travel_path, const string& travel, unique_ptr<AbstractAlgorithm>& algorithm) {
    string plan_path = getPath(travel_path, "ship_plan");
    int errors = Reader::readShipPlan(plan_path, _plan);
    algorithm -> readShipPlan(plan_path);
    return writeShipPlanErrors(error_path, errors, travel);
}

bool Simulation::readShipRoute(const string& error_path, const string& travel_path, const string& travel, unique_ptr<AbstractAlgorithm>& algorithm) {
    string route_path = getPath(travel_path, "route");
    int errors = Reader::readShipRoute(route_path, _route);
    algorithm -> readShipRoute(route_path);
    return writeShipRouteErrors(error_path, errors, travel);
}

void Simulation::getInstructionForCargo(const string &cargoPath, const string &output_path, const string &error_path, vector<unique_ptr<Container>>& containersAtPort, unique_ptr<AbstractAlgorithm> &algorithm) {
    int errors = Reader::readCargoLoad(cargoPath, containersAtPort);
    algorithm -> getInstructionsForCargo(cargoPath, output_path);
    writeCargoErrors(error_path, errors); // void because no fatal errors
}

pair<int, int> Simulation::sendInstructionsToCrane(vector<unique_ptr<Container>> containers, const string &instructions_path, const string &error_path, const string &sail_info) {
    vector<Operation> instructions = Reader::getInstructionsVector(instructions_path);
    return _crane.start(_plan, _route, std::move(containers), instructions, error_path, sail_info);
}

pair<int, int> Simulation::sail(unique_ptr<AbstractAlgorithm> &algorithm, const string &travel_path, const string& travel_name, const string& output_path, const string& error_path) {
    string travel_dir = travel_path + SUBDIR + travel_name;
    pair<int, int> results = {0, 0};
    for(const string& port: _route.getRoute()) {
        string cargoPath = getCargoPath(travel_dir, port);
        vector<unique_ptr<Container>> containersAtPort;
        string travel_output_path = createPortOutputFile(output_path, port);
        getInstructionForCargo(cargoPath, travel_output_path, error_path, containersAtPort, algorithm);
        string sail_info = "Algorithm: alg name, Travel: " + travel_name + " Port: " + port + "  : ";
        addPair(results, sendInstructionsToCrane(std::move(containersAtPort), travel_output_path, error_path, sail_info)); // TODO alg name
        _route.next();
    }
    return results;
}


void Simulation::start(const string &travel_path, const string &algorithm_path, const string &output_path) {
    if(!checkDirectories(travel_path, algorithm_path, output_path)) { return; }
    vector<unique_ptr<AbstractAlgorithm>> algorithms; // TODO: init algorithms
    string error_path = output_path + SUBDIR + "simulation.errors";
    string results_path = output_path + SUBDIR + "simulation.results";
    vector<string> travels = Reader::getTravels(travel_path);
    vector<tuple<string, vector<pair<int, int>>, int, int>> all_results;
    for(auto& alg: algorithms) {
        vector<pair<int, int>> results;
        for(int i = 0; i < travels.size(); i++) {
            if(!readShipPlan(error_path, travel_path, travels[i], alg) || !readShipRoute(error_path, travel_path, travels[i], alg)) { return; }
            string travel_output_path = output_path + SUBDIR + "<algorithm_name>" + travels[i] + "_crane_instructions"; //TODO algorithm name
            fs::create_directory(travel_output_path);
            results.emplace_back(sail(alg, travel_path, travels[i], travel_output_path, error_path)); // TODO: alg name
        }
        all_results.emplace_back("<algorithm_name>", results, sumResults(results, true), sumResults(results, false));
    }
    writeResults(results_path, all_results, travels);
}

