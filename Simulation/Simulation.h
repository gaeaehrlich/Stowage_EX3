#ifndef STOWAGE_SIMULATION_H
#define STOWAGE_SIMULATION_H

#include <string>
#include <iostream>
#include <algorithm>
#include <dlfcn.h>
#include "../Common/ShipPlan.h"
#include "../Common/ShipRoute.h"
#include "../Algorithm/AbstractAlgorithm.h"
#include "../Common/Reader.h"
#include "../Algorithm/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"
#include "Crane.h"

const string SUBDIR = "/";
const int FAILURE = -1;

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
    void start(const string& travel_path, const string& algorithm_path, const string& output_path);
    int sail(pair<string, unique_ptr<AbstractAlgorithm>>& algorithm, const string& travel_path, const string& travel_name, const string& output_path, const string& error_path);
    bool checkDirectories(const string& travel_path, const string& algorithm_path, const string& output_path);
    bool readShip(const string& error_path, const string& travel_path, const string& travel, pair<string, unique_ptr<AbstractAlgorithm>>& algorithm);
    string getCargoPath(const string& travel_path, const string& port);
    bool writeShipErrors(const string& error_path, int simulation_errors, int alg_errors, const string& travel, const string& alg_name);
    string getPath(const string &travel_dir, const string &search);
    void getInstructionForCargo(const string &cargoPath, const string &output_path, const string &error_path, vector<unique_ptr<Container>>& containersAtPort, pair<string, unique_ptr<AbstractAlgorithm>> &algorithm, const string& travel_name);
    void writeCargoErrors(const string& error_path,int simulation_errors, int alg_errors, vector<unique_ptr<Container>>& containersAtPort, const string& travel_name,  const string& alg_name);
    string createPortOutputFile(const string& output_path, const string& port);
    int sendInstructionsToCrane(vector<unique_ptr<Container>> containers, WeightBalanceCalculator& calculator, const string& instructions_path, const string &error_path, const string &sail_info);
    void writeResults(const string &path, const map<string, vector<int>>& results);
    int sumResults(const vector<int>& results, bool sumOrErr);
    string createTravelOutputFolder(const string& output_path, const string& alg_name, const string& travel_name);
    void scanTravelPath(const string& curr_travel_path, const string& error_path);
    string createResultsFile(const string& output_path, vector<string> travels);
    int countContainersOnPort(const string& id, vector<unique_ptr<Container>>& containersAtPort);
    void writeReaderErrors(std::ofstream& file, int simulation_errors, int alg_errors, vector<string> error_msg, const string& alg_name, int index = 0);
};


#endif //STOWAGE_SIMULATION_H
