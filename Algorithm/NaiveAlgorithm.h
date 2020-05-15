#ifndef STOWAGE_NAIVEALGORITHM_H
#define STOWAGE_NAIVEALGORITHM_H

#include <string>
#include <vector>
#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include "../Common/Container.h"
#include "../Common/ShipPlan.h"
#include "../Common/ShipRoute.h"
#include "../Common/Reader.h"
#include "../Common/WeightBalanceCalculator.h"
#include "../Common/Reader.h"
#include "AbstractAlgorithm.h"
#include "AlgorithmRegistration.h"


using std::unique_ptr;
using std::string;
using std::vector;
namespace fs = std::experimental::filesystem;

class NaiveAlgorithm: public AbstractAlgorithm {
    WeightBalanceCalculator _calc;
    vector<unique_ptr<Container>> _cargo_load;
    vector<unique_ptr<Container>> _temporary_unloaded;
    bool _invalid_travel = false;
    int _status = 0;

protected:
    ShipRoute _route;
    ShipPlan _plan;
// TODO : should be public?
public:
    //NaiveAlgorithm(ShipPlan plan, ShipRoute route, WeightBalanceCalculator calc);
    int readShipPlan(const string& full_path_and_file_name) override ;
    int readShipRoute(const string& full_path_and_file_name) override ;
    int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) override;
    int getInstructionsForCargo(
            const std::string& input_full_path_and_file_name,
            const std::string& output_full_path_and_file_name) override;
    void sortCargoLoad();
    void unloadInstructions(std::ofstream& file);
    int loadInstructions(std::ofstream& file, vector<unique_ptr<Container>>& list);
    Position findPosition(int weight);
    virtual Position findPosition(const string& new_dest);
    int rejectingContainer(unique_ptr<Container>& container);
    vector<Position> findContainersToUnload();
    void unloadContainersAbove(Position position, std::ofstream& file);
    string instructionToString(char instruction, const string& id, Position pos);
    int countContainersOnPort(const string& id);
};


#endif //STOWAGE_NAIVEALGORITHM_H