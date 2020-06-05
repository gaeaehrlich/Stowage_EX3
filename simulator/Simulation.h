#ifndef STOWAGE_SIMULATION_H
#define STOWAGE_SIMULATION_H

#include <string>
#include <iostream>
#include <algorithm>
#include <dlfcn.h>
#include <stdio.h>
#include <thread>
#include "../common/ShipPlan.h"
#include "../common/ShipRoute.h"
#include "../algorithm/AbstractAlgorithm.h"
#include "../common/Reader.h"
#include "../algorithm/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"
#include "Crane.h"
#include "ThreadPool.h"

const string SUBDIR = "/";
const int FAILURE = -1;
const string SEPARATOR = "---------------------------------------------------------------------\n";

using std::string;
using std::vector;
using std::unique_ptr;
using std::pair;
using std::tuple;

class Simulation {
    ShipPlan _plan;
    ShipRoute _route;
    Crane _crane;
    ThreadPool _pool;
    map<string, map<string, int>> _simulationResults;
    vector<pair<string, unique_ptr<AbstractAlgorithm>>> _allAlgorithms;

public:
    Simulation(int numThreads);
    void start(const string& travelPath, string& algorithmPath, string& outputPath, int numThreads);
    int sail(pair<string, unique_ptr<AbstractAlgorithm>>& algorithm, WeightBalanceCalculator& calculator, const string& travelPath, const string& travelName, const string& outputPath, const string& errorPath);
    int readShip(const string& errorPath, const string& travelPath, const string& travel, std::unordered_set<string>& invalidTravels);
    void algorithmReadShip(unique_ptr<AbstractAlgorithm>& algorithm, int simulationErrors, const string& errorPath, const string& travelPath, const string& travel, const string& algName);
    string getCargoPath(const string& travelDir, const string& port);
    bool writeTravelErrors(const string &errorPath, const string& travel, const string& simOrAlg, int simulationErrors, int algErrors = 0);
    string getPath(const string &travelDir, const string &search);
    void getInstructionForCargo(const string &cargoPath, const string &outputPath, const string &errorPath, vector<unique_ptr<Container>>& containersAtPort, pair<string, unique_ptr<AbstractAlgorithm>> &algorithm, const string& travelName);
    void writeCargoErrors(const string& errorPath, int simulationErrors, int algErrors, vector<unique_ptr<Container>>& containersAtPort, const string& travelName, const string& algName);
    string createPortOutputFile(const string& outputPath, const string& port);
    int sendInstructionsToCrane(vector<unique_ptr<Container>> containers, WeightBalanceCalculator& calculator, const string& instructions_path, const string &errorPath, const string &sailInfo);
    void writeResults(const string &path, vector<string> travels);
    int sumResults(const vector<int>& results, bool sumOrErr);
    string createTravelOutputFolder(const string& outputPath, const string& algName, const string& travelName);
    void scanTravelPath(const string& currTravelPath, const string& errorPath);
    string createResultsFile(const string& outputPath);
    int countContainersOnPort(const string& id, vector<unique_ptr<Container>>& containersAtPort);
    void writeReaderErrors(const string& errorPath, int simulationErrors, int algErrors, vector<string> errorMsg, const string& sailInfo, int index = 0, bool reportAlg = true);
    void setRelevantTravels(vector<string>& travels, const std::unordered_set<string>& invalid);
    void initPaths(const string& outputPath, string& errorPath, string& resultsPath);
    void runThread(pair<string, unique_ptr<AbstractAlgorithm>>& algorithm, int travelStatus, const string& travelPath, const string& travelName, const string& outputPath, const string& errorPath);
};


#endif //STOWAGE_SIMULATION_H
