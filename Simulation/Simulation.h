#ifndef STOWAGE_SIMULATION_H
#define STOWAGE_SIMULATION_H

#include <string>
#include <iostream>
#include <algorithm>
#include <io.h>
#include "../Common/WeightBalanceCalculator.h"
#include "../Common/ShipPlan.h"
#include "../Common/ShipRoute.h"
#include "../Algorithm/AbstractAlgorithm.h"
#include "../Common/Reader.h"
#include "../Common/AlgorithmRegistration.h"
#include "Crane.h"

const string SUBDIR = "\\";

using std::string;
using std::vector;
using std::unique_ptr;
using std::pair;

class Simulation {
    ShipPlan _plan;
    ShipRoute _route;
    Crane _crane;
    friend Crane;

public:
    void start(const string& travel_path, const string& algorithm_path, const string& output_path);
    int sail(unique_ptr<AbstractAlgorithm>& algorithm, const string& travel_path, int travelNumber, const string& output_path, const string& error_path);
    bool checkDirectories(const string& travel_path, const string& algorithm_path, const string& output_path);
    bool readShipPlan(const string& error_path, const string& travel_path, int travelNumber, unique_ptr<AbstractAlgorithm>& algorithm);
    bool readShipRoute(const string& error_path, const string& travel_path, int travelNumber, unique_ptr<AbstractAlgorithm>& algorithm);
    string getCargoPath(const string& travel_path, const string& port);
    bool writeShipPlanErrors(const string& error_path, int errors);
    bool writeShipRouteErrors(const string& error_path, int errors);
    string getPath(const string &travel_dir, const string &search);
    void getInstructionForCargo(const string &cargoPath, const string &output_path, const string &error_path, vector<unique_ptr<Container>>& containersAtPort, unique_ptr<AbstractAlgorithm> &algorithm);
    bool writeCargoErrors(const string& error_path, int errors);
    string createPortOutputFile(const string& output_path, const string& port, int travelNumber);
    pair<int, int> sendInstructionsToCrane(const string& instructions_path, const string &error_path, const string &sail_info);
};


#endif //STOWAGE_SIMULATION_H
