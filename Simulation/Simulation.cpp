#include "Simulation.h"


bool Simulation::readShip(const string &error_path, const string &travel_path, const string &travel, pair<string, unique_ptr<AbstractAlgorithm>> &algorithm) {
    string route_path = getPath(travel_path, "route");
    string plan_path = getPath(travel_path, "ship_plan");
    int simulation_errors = Reader::readShipRoute(route_path, _route) | Reader::readShipPlan(plan_path, _plan);
    int alg_errors = algorithm.second -> readShipRoute(route_path) | algorithm.second -> readShipPlan(plan_path);
    return writeShipErrors(error_path, simulation_errors, alg_errors, travel, algorithm.first);
}

void Simulation::getInstructionForCargo(const string &cargoPath, const string &output_path, const string &error_path, vector<unique_ptr<Container>>& containersAtPort, pair<string, unique_ptr<AbstractAlgorithm>> &algorithm, const string& travel_name) {
    int simulation_errors = Reader::readCargoLoad(cargoPath, containersAtPort);
    int alg_errors = algorithm.second -> getInstructionsForCargo(cargoPath, output_path);
    writeCargoErrors(error_path, simulation_errors, alg_errors, containersAtPort, travel_name, algorithm.first); // void because no fatal errors
}

int Simulation::sendInstructionsToCrane(vector<unique_ptr<Container>> containers, WeightBalanceCalculator& calculator, const string &instructions_path, const string &error_path, const string &sail_info) {
    vector<Operation> instructions = Reader::getInstructionsVector(instructions_path);
    return _crane.start(_plan, _route, calculator, std::move(containers), instructions, error_path, sail_info);
}

int Simulation::sail(pair<string, unique_ptr<AbstractAlgorithm>> &algorithm, const string &travel_path, const string& travel_name, const string& output_path, const string& error_path) {
    int results = 0;
    bool failed = false;
    scanTravelPath(travel_path, error_path);
    string travel_output_path = createTravelOutputFolder(output_path, algorithm.first, travel_name);
    string plan_path = getPath(travel_path, "ship_plan");
    WeightBalanceCalculator calculator;
    calculator.readShipPlan(plan_path);
    algorithm.second -> setWeightBalanceCalculator(calculator);
    for(const string& port: _route.getRoute()) {
        std::cout << "entering port: " << port << std::endl;
        string cargoPath = getCargoPath(travel_path, port);
        vector<unique_ptr<Container>> containersAtPort;
        string port_output_path = createPortOutputFile(travel_output_path, port);
        getInstructionForCargo(cargoPath, port_output_path, error_path, containersAtPort, algorithm, travel_name);
        string sail_info = "Algorithm: " + algorithm.first + ", Travel: " + travel_name + " Port: " + port + "  : \n";
        std::cout << "before crane"<< std::endl;
        int num_op = sendInstructionsToCrane(std::move(containersAtPort), calculator, port_output_path, error_path, sail_info);
        std::cout << "after crane" << std::endl;
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
        auto algorithms = registrar.getAlgorithms();
        if(algorithms.empty()) { return; }
        for(auto& alg: algorithms) {
            if(!readShip(error_path, curr_travel_path, travel_name, alg)) {
                alg_results[alg.first].push_back(0);
            }
            else {
                alg_results[alg.first].emplace_back(sail(alg, curr_travel_path, travel_name, output_path, error_path));
            }
        }
    }
    writeResults(results_path, alg_results, travels);
}
