#include "Simulation.h"

Simulation::Simulation(int numThreads) : _pool(numThreads - 1){
    _pool.startThreads();
}

void Simulation::readShip(Stowage& stowage, const string &travelPath, const string &travel, unique_ptr<AbstractAlgorithm> &algorithm, const string& algName) {
    string routePath = getPath(travelPath, "route");
    string planPath = getPath(travelPath, "ship_plan");
    pair<int, vector<string>> route = Reader::readShipRoute(routePath);
    pair<int, pair<int, map< pair<int,int>, int >>> shipPlan = Reader::readShipPlan(planPath);
    stowage._route = ShipRoute(route.second);
    stowage._plan = ShipPlan(shipPlan.second.first, shipPlan.second.second);
    int readRoute = route.first, readPlan = shipPlan.first;
    int simulationErrors = readRoute | readPlan;
    int algErrors = algorithm -> readShipRoute(routePath) | algorithm -> readShipPlan(planPath);
    writeShipErrors(simulationErrors, algErrors, travel, algName);
}

void Simulation::getInstructionForCargo(Stowage& stowage, const string &cargoPath, const string &outputPath, vector<unique_ptr<Container>>& containersAtPort, const string &algName, unique_ptr<AbstractAlgorithm>& algorithm, const string& travelName) {
    int simulationErrors = Reader::readCargoLoad(cargoPath, containersAtPort);
    int algErrors = algorithm -> getInstructionsForCargo(cargoPath, outputPath);
    writeCargoErrors(stowage, simulationErrors, algErrors, containersAtPort, travelName, algName); // void because no fatal errors
}

int Simulation::sendInstructionsToCrane(Stowage& stowage, vector<unique_ptr<Container>> containers, WeightBalanceCalculator& calculator, const string &instructions_path, const pair<string, string> &sailInfo) {
    vector<Operation> instructions = Reader::getInstructionsVector(instructions_path);
    return stowage._crane.start(stowage._plan, stowage._route, calculator, std::move(containers), instructions, sailInfo);
}

int Simulation::sail(Stowage& stowage, const string &algName, unique_ptr<AbstractAlgorithm>& algorithm, WeightBalanceCalculator& calculator, const string &travelPath, const string& travelName, const string& outputPath) {
    bool failed = false;
    int numOp = 0;
    string travelOutputPath = createTravelOutputFolder(outputPath, algName, travelName);
    for(const string& port: stowage._route.getRoute()) {
        string cargoPath = getCargoPath(stowage, travelPath, port);
        vector<unique_ptr<Container>> containersAtPort;
        string portOutputPath = createPortOutputFile(stowage, travelOutputPath, port);
        getInstructionForCargo(stowage, cargoPath, portOutputPath, containersAtPort, algName, algorithm, travelName);
        pair<string, string> sailInfo = {algName, travelName};// SEPARATOR + "***** ALGORITHM: " + algName + ", TRAVEL: " + travelName + " PORT: " + port + " *****\n";
        int result = sendInstructionsToCrane(stowage, std::move(containersAtPort), calculator, portOutputPath, sailInfo);
        if(result == FAILURE) { failed = true; }
        numOp += result;
        stowage._route.next();
    }
    return failed ? FAILURE : numOp;
}


void Simulation::runThread(const string &algName, unique_ptr<AbstractAlgorithm>& algorithm, const string& travelPath, const string& travelName, const string& outputPath) {
        Stowage stowage(_simulationErrors);
        readShip(stowage, travelPath, travelName, algorithm, algName);
        WeightBalanceCalculator calculator;
        calculator.readShipPlan(getPath(travelPath, "ship_plan"));
        algorithm -> setWeightBalanceCalculator(calculator);
        int numOp = sail(stowage, algName, algorithm, calculator, travelPath, travelName, outputPath);
        _simulationResults[algName][travelName] = numOp;
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
        string currTravelPath = travelPath + SUBDIR + travelName;
        if(!isTravelValid(invalidTravels, currTravelPath, travelName, errorPath)) { continue; }
        getAlgorithms(registrar, travelName);
        auto& algorithms = _allAlgorithms[travelName];
        if(algorithms.empty()) { return; }
        for(auto& alg: algorithms) {
            auto algName = alg.first;
            auto& algorithm = alg.second;
            if(numThreads == 1) {
                runThread(algName, algorithm, currTravelPath, travelName, outputPath);
            }
            else {
                _pool.addTask([this, algName, &algorithm, currTravelPath, travelName, outputPath, errorPath]() {
                    runThread(algName, algorithm, currTravelPath, travelName, outputPath);
                });
            }
        }
    }
    _pool.joinThreads();
    _allAlgorithms.clear();
    writeErrors(errorPath);
    setRelevantTravels(travels, invalidTravels);
    writeResults(resultsPath, travels);
}

void Simulation::getAlgorithms(AlgorithmRegistrar &registrar, const string& travelName) {
    auto algorithms = registrar.getAlgorithms();
    for(auto& alg: algorithms) {
        _allAlgorithms[travelName].push_back(std::move(alg));
    }
}

void Simulation::writeErrors(const string &errorPath) {
    std::ofstream file;
    for(auto const& algorithmErrors: _simulationErrors) {
        for(auto const& travelErrors: algorithmErrors.second) {
            string sailInfo = SEPARATOR + "Algorithm: " + algorithmErrors.first + " on travel: " + travelErrors.first + " had the following errors:\n";
            file.open(errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << sailInfo << travelErrors.second;
            file.close();
        }
    }
}
