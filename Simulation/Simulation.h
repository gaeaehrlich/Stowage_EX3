#ifndef STOWAGE_SIMULATION_H
#define STOWAGE_SIMULATION_H

#include <string>
#include <iostream>
#include <algorithm>
#include "../Common/WeightBalanceCalculator.h"
#include "../Common/ShipPlan.h"
#include "../Common/ShipRoute.h"
#include "../Algorithm/AbstractAlgorithm.h"
#include "../Common/Reader.h"
#include "../Common/AlgorithmRegistration.h"

using std::string;
using std::vector;
using std::unique_ptr;

class Simulation {
    ShipPlan _plan;
    ShipRoute _route;

public:
    void start(const string& travel_path, const string& algorithm_path, const string& output_path);
    int sail(unique_ptr<AbstractAlgorithm>& algorithm, const string& travel_path, int travelNumber, const string& output_path);
    bool checkDirectories(const string& travel_path, const string& algorithm_path, const string& output_path);
    bool readShipPlan(const string& travel_path, int travelNumber, unique_ptr<AbstractAlgorithm>& algorithm);
    bool readShipRoute(const string& travel_path, int travelNumber, unique_ptr<AbstractAlgorithm>& algorithm);
};


#endif //STOWAGE_SIMULATION_H
