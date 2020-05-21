#include "Simulation.h"


bool Simulation::readShip(const string &errorPath, const string &travelPath, const string &travel, pair<string, unique_ptr<AbstractAlgorithm>> &algorithm) {
    string routePath = getPath(travelPath, "route");
    string planPath = getPath(travelPath, "ship_plan");
    int simulationErrors = Reader::readShipRoute(routePath, _route) | Reader::readShipPlan(planPath, _plan);
    int algErrors = algorithm.second -> readShipRoute(routePath) | algorithm.second -> readShipPlan(planPath);
    return writeShipErrors(errorPath, simulationErrors, algErrors, travel, algorithm.first);
}

void Simulation::getInstructionForCargo(const string &cargoPath, const string &outputPath, const string &errorPath, vector<unique_ptr<Container>>& containersAtPort, pair<string, unique_ptr<AbstractAlgorithm>> &algorithm, const string& travelName) {
    int simulationErrors = Reader::readCargoLoad(cargoPath, containersAtPort);
    int algErrors = algorithm.second -> getInstructionsForCargo(cargoPath, outputPath);
    writeCargoErrors(errorPath, simulationErrors, algErrors, containersAtPort, travelName, algorithm.first); // void because no fatal errors
}

int Simulation::sendInstructionsToCrane(vector<unique_ptr<Container>> containers, WeightBalanceCalculator& calculator, const string &instructions_path, const string &errorPath, const string &sailInfo) {
    vector<Operation> instructions = Reader::getInstructionsVector(instructions_path);
    return _crane.start(_plan, _route, calculator, std::move(containers), instructions, errorPath, sailInfo);
}

int Simulation::sail(pair<string, unique_ptr<AbstractAlgorithm>> &algorithm, const string &travelPath, const string& travelName, const string& outputPath, const string& errorPath) {
    int results = 0;
    bool failed = false;
    scanTravelPath(travelPath, errorPath);
    string travel_output_path = createTravelOutputFolder(outputPath, algorithm.first, travelName);
    string plan_path = getPath(travelPath, "ship_plan");
    WeightBalanceCalculator calculator;
    calculator.readShipPlan(plan_path);
    algorithm.second -> setWeightBalanceCalculator(calculator);
    for(const string& port: _route.getRoute()) {
        std::cout << "entering port: " << port << std::endl;
        string cargoPath = getCargoPath(travelPath, port);
        vector<unique_ptr<Container>> containersAtPort;
        string portOutputPath = createPortOutputFile(travel_output_path, port);
        getInstructionForCargo(cargoPath, portOutputPath, errorPath, containersAtPort, algorithm, travelName);
        string sailInfo = "---------------------------------------------------------------------\n***** ALGORITHM: " + algorithm.first + ", TRAVEL: " + travelName + " PORT: " + port + " *****\n";
        int numOp = sendInstructionsToCrane(std::move(containersAtPort), calculator, portOutputPath, errorPath, sailInfo);
        if(numOp == FAILURE) { failed = true; }
        results += numOp;
        _route.next();
    }
    return failed ? FAILURE : results;
}


void Simulation::start(const string &travelPath, string &algorithmPath, string &outputPath) {
    if(!checkDirectories(travelPath, algorithmPath, outputPath)) { return; }
    string errorPath = outputPath + SUBDIR + "simulation.errors";
    vector<string> travels = Reader::getTravels(travelPath);
    vector<std::function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
    auto& registrar = AlgorithmRegistrar::getInstance();
    registrar.loadAlgorithmFromFile(algorithmPath, errorPath);
    string resultsPath = createResultsFile(outputPath, travels);
    map<string, vector<int>> algResults;
    for(const auto& travelName : travels) {
        string currTravelPath = travelPath + SUBDIR + travelName;
        auto algorithms = registrar.getAlgorithms();
        if(algorithms.empty()) { return; }
        for(auto& alg: algorithms) {
            std::cout << "strating travel: " << travelName << " with algorithm: " << alg.first << std::endl;
            if(!readShip(errorPath, currTravelPath, travelName, alg)) {
                continue;
            }
            else {
                algResults[alg.first].emplace_back(sail(alg, currTravelPath, travelName, outputPath, errorPath));
            }
        }
    }
    writeResults(resultsPath, algResults);
}
