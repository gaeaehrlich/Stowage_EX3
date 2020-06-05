#include "Simulation.h"

Simulation::Simulation(int numThreads) : _pool(numThreads - 1){}

int Simulation::readShip(const string &errorPath, const string &travelPath, const string &travel, std::unordered_set<string>& invalidTravels) {
    string routePath = getPath(travelPath, "route");
    string planPath = getPath(travelPath, "ship_plan");
    int errors = Reader::readShipRoute(routePath, _route) | Reader::readShipPlan(planPath, _plan);
    bool fatal = writeTravelErrors(errorPath, travel, "Simulation", errors);
    if(fatal) invalidTravels.insert(travel);
    return fatal ? FAILURE : errors;
}

void Simulation::algorithmReadShip(unique_ptr<AbstractAlgorithm>& algorithm, int simulationErrors, const string& errorPath, const string& travelPath, const string& travel, const string& algName) {
    std::cout << "in read ship for alg " << algName << " for travel " << travel << std::endl;
    string routePath = getPath(travelPath, "route");
    string planPath = getPath(travelPath, "ship_plan");
    int algErrors = algorithm -> readShipRoute(routePath) | algorithm -> readShipPlan(planPath);
    writeTravelErrors(errorPath, travel, "ALGORITHM: " + algName, simulationErrors, algErrors);
    std::cout << "after read ship" << std::endl;
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

int Simulation::sail(pair<string, unique_ptr<AbstractAlgorithm>> &algorithm, WeightBalanceCalculator& calculator, const string &travelPath, const string& travelName, const string& outputPath, const string& errorPath) {
    bool failed = false;
    int numOp = 0;
    string travelOutputPath = createTravelOutputFolder(outputPath, algorithm.first, travelName);
    for(const string& port: _route.getRoute()) {
        std::cout << "entering port: " << port << std::endl;
        string cargoPath = getCargoPath(travelPath, port);
        vector<unique_ptr<Container>> containersAtPort;
        string portOutputPath = createPortOutputFile(travelOutputPath, port);
        getInstructionForCargo(cargoPath, portOutputPath, errorPath, containersAtPort, algorithm, travelName);
        string sailInfo = SEPARATOR + "***** ALGORITHM: " + algorithm.first + ", TRAVEL: " + travelName + " PORT: " + port + " *****\n";
        int result = sendInstructionsToCrane(std::move(containersAtPort), calculator, portOutputPath, errorPath, sailInfo);
        if(result == FAILURE) { failed = true; }
        numOp += result;
        _route.next();
    }
    return failed ? FAILURE : numOp;
}


void Simulation::runThread(pair<string, unique_ptr<AbstractAlgorithm>> algorithm, int travelStatus, const string& travelPath, const string& travelName, const string& outputPath, const string& errorPath) {
    std::cout << "inserting task" << std::endl;
    _pool.addTask([this, algorithm = std::move(algorithm), travelPath, travelName, travelStatus, outputPath, errorPath]() mutable {
        std::cout << "staring thread" << std::endl;
        algorithmReadShip(algorithm.second, travelStatus, errorPath, travelPath, travelName, algorithm.first);
        std::cout << SEPARATOR << "starting travel: " << travelName << " with algorithm: " << algorithm.first << std::endl;
        WeightBalanceCalculator calculator;
        calculator.readShipPlan(getPath(travelPath, "ship_plan"));
        algorithm.second -> setWeightBalanceCalculator(calculator);
        int numOp = sail(algorithm, calculator, travelPath, travelName, outputPath, errorPath);
        _simulationResults[algorithm.first][travelName] = numOp;
    });
}


void Simulation::start(const string &travelPath, string &algorithmPath, string &outputPath, int numThreads) {
    string errorPath, resultsPath;
    initPaths(outputPath, errorPath, resultsPath);
    vector<string> travels = Reader::getTravels(travelPath);
    vector<std::function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
    auto& registrar = AlgorithmRegistrar::getInstance();
    registrar.loadAlgorithmFromFile(algorithmPath, errorPath);
    std::unordered_set<string> invalidTravels;
    for(auto& travelName : travels) {
        int travelStatus;
        string currTravelPath = travelPath + SUBDIR + travelName;
        if((travelStatus = readShip(errorPath, currTravelPath, travelName, invalidTravels)) == FAILURE) {
            continue;
        }
        auto algorithms = registrar.getAlgorithms();
        if(algorithms.empty()) { return; } // TODO: do we want this?
        scanTravelPath(currTravelPath, errorPath);
        for(auto& alg: algorithms) {
            runThread(std::move(alg), travelStatus, currTravelPath, travelName, outputPath, errorPath);
        }
    }
    _pool.joinThreads();
    setRelevantTravels(travels, invalidTravels);
    writeResults(resultsPath, travels);
}
