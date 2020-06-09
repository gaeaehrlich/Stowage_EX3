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
    map<string, pair<Stowage, int>> _travels; // travel name, Stowage, read status
    vector<string> _readerErrors = {"\tship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n",
                                    "\tship plan: a given position exceeds the X/Y ship limits (ignored)\n",
                                    "\tship plan: bad line format after first line or duplicate x,y appearance with same data (ignored)\n",
                                    "\tship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n",
                                    "\tship plan: travel error - duplicate x,y appearance with different data (cannot run this travel)\n",
                                    "\ttravel route: a port appears twice or more consecutively (ignored)\n",
                                    "\ttravel route: bad port symbol format (ignored)\n",
                                    "\ttravel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n",
                                    "\ttravel route: travel error - file with only a single valid port (cannot run this travel)\n",
                                    "duplicate ID on port (ID rejected)\n",
                                    "ID already on ship (ID rejected)\n",
                                    "bad line format, missing or bad weight (ID rejected)\n",
                                    "bad line format, missing or bad port destination (ID rejected)\n",
                                    "bad line format, ID cannot be read (ignored)\n",
                                    "illegal ID check ISO 6346 (ID rejected)\n",
                                    "file cannot be read altogether (assuming no cargo to be loaded at this port)\n",
                                    "last port has waiting containers (ignored)\n",
                                    "total containers amount exceeds ship capacity (rejecting far containers)\n"
    };

public:
    explicit Simulation(int numThreads);
    void start(const string& travelPath, string& algorithmPath, string& outputPath, int numThreads);
    int sail(Stowage& stowage, const string &algName, unique_ptr<AbstractAlgorithm>& algorithm, WeightBalanceCalculator& calculator, const string& travelPath, const string& travelName, const string& outputPath);
    void readShip(const string& travelPath, const string& travel, unique_ptr<AbstractAlgorithm> &algorithm, const string& algName);
    string getCargoPath(Stowage& stowage, const string& travelDir, const string& port);
    void writeShipErrors(int algErrors, const string& travel, const string& algName);
    void writeReaderErrors(int simulationErrors, int algErrors, const pair<string, string>& sailInfo, const string& portInfo = "");
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
    void setRelevantTravels(vector<string>& travels, const std::unordered_set<string>& invalid);
    void initPaths(const string& outputPath, string& errorPath, string& resultsPath);
    void runThread(const string &algName, std::function<unique_ptr<AbstractAlgorithm>()> algFunc, const string& travelPath, const string& travelName, const string& outputPath);
    void readTravel(const string& currTravelPath, const string& travelName, const string &errorPath);
    bool isInvalidTravel(const string& travelName);
    void writeErrors(const string& errorPath);
    map<string, std::function<unique_ptr<AbstractAlgorithm>()>> loadAlgorithmsFromFile(const string &dirPath, const string& errorPath);
    void writeRegistrarErrors(const string& path, const vector<pair<string, string>>& errors);
};


#endif //STOWAGE_SIMULATION_H
