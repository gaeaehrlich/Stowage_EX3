#ifndef STOWAGE_SIMULATION_H
#define STOWAGE_SIMULATION_H

#include <string>
#include <iostream>
#include <algorithm>
#include <dlfcn.h>
#include <stdio.h>
#include "../common/ShipPlan.h"
#include "../common/ShipRoute.h"
#include "../algorithm/AbstractAlgorithm.h"
#include "../common/Reader.h"
#include "../algorithm/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"
#include "Crane.h"

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

public:
    void start(const string& travelPath, string& algorithmPath, string& outputPath);
    int sail(pair<string, unique_ptr<AbstractAlgorithm>>& algorithm, const string& travelPath, const string& travelName, const string& outputPath, const string& errorPath);
    bool checkDirectories(const string& travelPath, string& algorithmPath, string& outputPath);
    bool readShip(const string& errorPath, const string& travelPath, const string& travel, pair<string, unique_ptr<AbstractAlgorithm>>& algorithm);
    string getCargoPath(const string& travelDir, const string& port);
    bool writeShipErrors(const string& errorPath, int simulationErrors, int algErrors, const string& travel, const string& algName);
    string getPath(const string &travelDir, const string &search);
    void getInstructionForCargo(const string &cargoPath, const string &outputPath, const string &errorPath, vector<unique_ptr<Container>>& containersAtPort, pair<string, unique_ptr<AbstractAlgorithm>> &algorithm, const string& travelName);
    void writeCargoErrors(const string& errorPath, int simulationErrors, int algErrors, vector<unique_ptr<Container>>& containersAtPort, const string& travelName, const string& algName);
    string createPortOutputFile(const string& outputPath, const string& port);
    int sendInstructionsToCrane(vector<unique_ptr<Container>> containers, WeightBalanceCalculator& calculator, const string& instructions_path, const string &errorPath, const string &sailInfo);
    void writeResults(const string &path, const map<string, vector<int>>& results, vector<string> travels);
    int sumResults(const vector<int>& results, bool sumOrErr);
    string createTravelOutputFolder(const string& outputPath, const string& algName, const string& travelName);
    void scanTravelPath(const string& currTravelPath, const string& errorPath);
    string createResultsFile(const string& outputPath);
    int countContainersOnPort(const string& id, vector<unique_ptr<Container>>& containersAtPort);
    void writeReaderErrors(const string& errorPath, int simulationErrors, int algErrors, vector<string> errorMsg, const string& algName, const string& sailInfo, int index = 0);
    void setRelevantTravels(vector<string>& travels, const std::unordered_set<string>& invalid);
};


#endif //STOWAGE_SIMULATION_H
