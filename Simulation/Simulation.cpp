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

int Simulation::sendInstructionsToCrane(vector<unique_ptr<Container>> containers, WeightBalanceCalculator& calculator, const string &instructions_path, const string &error_path, const string &sail_info) {
    vector<Operation> instructions = Reader::getInstructionsVector(instructions_path);
    return _crane.start(_plan, _route, calculator, std::move(containers), instructions, error_path, sail_info);
}

int Simulation::sail(unique_ptr<AbstractAlgorithm> &algorithm, const string& alg_name, const string &travel_path, const string& travel_name, const string& output_path, const string& error_path) {
    int results = 0;
    bool failed = false;
    string travel_output_path = createTravelOutputFolder(output_path, alg_name, travel_name);
    string plan_path = getPath(travel_path, "ship_plan");
    WeightBalanceCalculator calculator;
    calculator.readShipPlan(plan_path);
    algorithm -> setWeightBalanceCalculator(calculator);
    for(const string& port: _route.getRoute()) {
        string cargoPath = getCargoPath(travel_path, port);
        vector<unique_ptr<Container>> containersAtPort;
        string port_output_path = createPortOutputFile(travel_output_path, port);
        getInstructionForCargo(cargoPath, port_output_path, error_path, containersAtPort, algorithm);
        string sail_info = "Algorithm: " + alg_name + ", Travel: " + travel_name + " Port: " + port + "  : \n";
        int num_op = sendInstructionsToCrane(std::move(containersAtPort), calculator, port_output_path, error_path, sail_info);
        if(num_op == FAILURE) { failed = true; }
        results += num_op;
        _route.next();
    }
    return failed ? FAILURE : results;
}


void Simulation::start(const string &travel_path, const string &algorithm_path, const string &output_path) {
    if(!checkDirectories(travel_path, algorithm_path, output_path)) { return; }
    string error_path = output_path + SUBDIR + "simulation.errors";
    vector<string> travels = Reader::getTravels(travel_path);
    vector<std::function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
    auto& registrar = AlgorithmRegistrar::getInstance();
    registrar.loadAlgorithmFromFile(algorithm_path, error_path);
    string results_path = createResultsFile(output_path, travels);
    map<string, vector<int>> alg_results;
    for(const auto& travel_name : travels) {
        string curr_travel_path = travel_path + SUBDIR + travel_name;
        scanTravelPath(curr_travel_path, error_path);
        auto algorithms = registrar.getAlgorithms();
        if(algorithms.empty()) { return; }
        for(auto& alg: algorithms) {
            if(!readShipPlan(error_path, curr_travel_path, travel_name, alg.second) || !readShipRoute(error_path, curr_travel_path, travel_name, alg.second)) { continue; }
            alg_results[alg.first].emplace_back(sail(alg.second, alg.first, curr_travel_path, travel_name, output_path, error_path));
        }
    }
    writeResults(results_path, alg_results, travels);
}

