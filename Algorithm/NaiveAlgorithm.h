#ifndef STOWAGE_NAIVEALGORITHM_H
#define STOWAGE_NAIVEALGORITHM_H

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "../Common/Container.h"
#include "../Common/ShipPlan.h"
#include "../Common/ShipRoute.h"
#include "../Common/Reader.h"
#include "../Common/WeightBalanceCalculator.h"
#include "../Common/Reader.h"
#include "AbstractAlgorithm.h"


using std::unique_ptr;
using std::string;
using std::vector;
namespace fs = std::filesystem;

class NaiveAlgorithm: public AbstractAlgorithm {
    // TODO : should be public?
    ShipPlan _plan;
    ShipRoute _route;
    WeightBalanceCalculator _calc; // TODO: where to put weightbalance? common or algorithm?
    vector<unique_ptr<Container>> _cargo_load;
    vector<unique_ptr<Container>> _temporary_unloaded;

public:
    //NaiveAlgorithm(ShipPlan plan, ShipRoute route, WeightBalanceCalculator calc);
    int readShipPlan(const std::string& full_path_and_file_name) override ;
    int readShipRoute(const std::string& full_path_and_file_name) override ;
    int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) override;
    int getInstructionsForCargo(
            const std::string& input_full_path_and_file_name,
            const std::string& output_full_path_and_file_name) override;
    bool cmpContainers(const unique_ptr<Container>& a, const unique_ptr<Container>& b);
    void sortCargoLoad();
    void unloadInstructions(const string& output_path);
    void loadInstructions(const string& output_path, vector<unique_ptr<Container>>& list);
    Position findPosition();
    int rejectingContainer(const string& id, string dest);
    vector<Position> findContainersToUnload();
    void unloadContainersAbove(Position position, std::ofstream& file);
    vector<unique_ptr<Container>> readCargoLoad(const string& path);
    string instructionToString(char instruction, const string& id, Position pos);
};


#endif //STOWAGE_NAIVEALGORITHM_H
