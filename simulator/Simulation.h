#ifndef STOWAGE_SIMULATION_H
#define STOWAGE_SIMULATION_H

#include <string>
#include <iostream>
#include <algorithm>
#include <dlfcn.h>
#include <stdio.h>
#include <thread>
#include "../algorithm/AbstractAlgorithm.h"
#include "../common/Reader.h"
#include "../algorithm/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"
#include "ThreadPool.h"
#include "Stowage.h"


const string SUBDIR = "/";
const int FAILURE = -1;
const string SEPARATOR = "---------------------------------------------------------------------\n";

using std::string;
using std::vector;
using std::unique_ptr;
using std::pair;
using std::tuple;

class Simulation {
    ThreadPool _pool;
    map<string, map<string, int>> _simulationResults; // algorithm name, travel name, number of operations
    map<string, map<string, string>> _simulationErrors; // algorithm name, travel name, errors
    map<string, vector<pair<string, unique_ptr<AbstractAlgorithm>>>> _allAlgorithms; // travel name, algorithm name, algorithm


public:
    explicit Simulation(int numThreads);
    void start(const string& travelPath, string& algorithmPath, string& outputPath, int numThreads);
    int sail(Stowage& stowage, const string &algName, unique_ptr<AbstractAlgorithm>& algorithm, WeightBalanceCalculator& calculator, const string& travelPath, const string& travelName, const string& outputPath);
    void readShip(Stowage& stowage, const string& travelPath, const string& travel, unique_ptr<AbstractAlgorithm> &algorithm, const string& algName);
    string getCargoPath(Stowage& stowage, const string& travelDir, const string& port);
    void writeShipErrors(int simulationErrors, int algErrors, const string& travel, const string& algName);
    string getPath(const string &travelDir, const string &search);
    void getInstructionForCargo(Stowage& stowage, const string &cargoPath, const string &outputPath, vector<unique_ptr<Container>>& containersAtPort, const string &algName, unique_ptr<AbstractAlgorithm>& algorithm, const string& travelName);
    void writeCargoErrors(Stowage& stowage, int simulationErrors, int algErrors, vector<unique_ptr<Container>>& containersAtPort, const string& travelName, const string& algName);
    string createPortOutputFile(Stowage& stowage, const string& outputPath, const string& port);
    int sendInstructionsToCrane(Stowage& stowage, vector<unique_ptr<Container>> containers, WeightBalanceCalculator& calculator, const string& instructions_path, const pair<string, string> &sailInfo);
    void writeResults(const string &path, vector<string> travels);
    int sumResults(const vector<int>& results, bool sumOrErr);
    string createTravelOutputFolder(const string& outputPath, const string& algName, const string& travelName);
    void scanTravelPath(ShipRoute& route, const string& currTravelPath, const string& errorPath);
    string createResultsFile(const string& outputPath);
    int countContainersOnPort(const string& id, vector<unique_ptr<Container>>& containersAtPort);
    void writeReaderErrors(int simulationErrors, int algErrors, vector<string> errorMsg, const pair<string, string>& sailInfo, int index = 0);
    void setRelevantTravels(vector<string>& travels, const std::unordered_set<string>& invalid);
    void initPaths(const string& outputPath, string& errorPath, string& resultsPath);
    void runThread(const string &algName, unique_ptr<AbstractAlgorithm>& algorithm, const string& travelPath, const string& travelName, const string& outputPath);
    bool isTravelValid(std::unordered_set<string>& invalidTravels, const string& currTravelPath, const string& travelName, const string& errorPath);
    void getAlgorithms(AlgorithmRegistrar& registrar, const string& travelName);
    void writeErrors(const string& errorPath);
};


#endif //STOWAGE_SIMULATION_H
