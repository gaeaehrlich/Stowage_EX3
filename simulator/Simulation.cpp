#include "Simulation.h"

Simulation::Simulation(int numThreads) : _pool(numThreads - 1){
    _pool.startThreads();
}

void Simulation::readShip(const string &travelPath, const string &travel, unique_ptr<AbstractAlgorithm> &algorithm, const string& algName) {
    string routePath = getPath(travelPath, "route");
    string planPath = getPath(travelPath, "ship_plan");
    int algErrors = algorithm -> readShipRoute(routePath) | algorithm -> readShipPlan(planPath);
    writeShipErrors(algErrors, travel, algName);
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


void Simulation::runThread(const string &algName,std::function<unique_ptr<AbstractAlgorithm>()> algFunc, const string& travelPath, const string& travelName, const string& outputPath) {
        Stowage stowage = _travels[travelName].first.getEmptyCopy();
        auto algorithm = algFunc();
        readShip(travelPath, travelName, algorithm, algName);
        WeightBalanceCalculator calculator;
        calculator.readShipPlan(getPath(travelPath, "ship_plan"));
        algorithm -> setWeightBalanceCalculator(calculator);
        int numOp = sail(stowage, algName, algorithm, calculator, travelPath, travelName, outputPath);
        _simulationResults[algName][travelName] = numOp;
        if(!stowage._crane.getCraneErrors().empty()) _simulationErrors[algName][travelName].append(stowage._crane.getCraneErrors());
}


void Simulation::start(const string &travelPath, string &algorithmPath, string &outputPath, int numThreads) {
    string errorPath, resultsPath;
    initPaths(outputPath, errorPath, resultsPath);
    vector<string> travels = Reader::getTravels(travelPath);
    auto algorithms = loadAlgorithmsFromFile(algorithmPath, errorPath);
    if(algorithms.empty()) {
        std::cout << "Error: no algorithms were loaded. Run terminated." << std::endl;
        return;
    }
    for(auto& travelName : travels) {
        string currTravelPath = travelPath + SUBDIR + travelName;
        readTravel(currTravelPath, travelName, errorPath);
        if(isInvalidTravel(travelName)) { continue; }
        for(auto& algorithm: algorithms) {
            auto algName = algorithm.first;
            auto algFunc = algorithm.second;
            if(numThreads == 1) {
                runThread(algName, algFunc, currTravelPath, travelName, outputPath);
            }
            else {
                _pool.addTask([this, algName, algFunc, currTravelPath, travelName, outputPath, errorPath]() {
                    runThread(algName, algFunc, currTravelPath, travelName, outputPath);
                });
            }
        }
    }
    _pool.joinThreads();
    writeErrors(errorPath);
    writeResults(resultsPath, travels);
}

