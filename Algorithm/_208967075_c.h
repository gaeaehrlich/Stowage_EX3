#ifndef STOWAGE__208967075_C_H
#define STOWAGE__208967075_C_H


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
#include "AlgorithmRegistration.h"


using std::unique_ptr;
using std::string;
using std::vector;
namespace fs = std::filesystem;

class _208967075_c: public AbstractAlgorithm {
    vector<unique_ptr<Container>> _cargo_load;

protected:
    WeightBalanceCalculator _calc;
    ShipRoute _route;
    ShipPlan _plan;

public:
    ~_208967075_c() override;
    void finishedPort();
    int readCargoLoad(const string& full_path_and_file_name);
    int readShipPlan(const string& full_path_and_file_name) override ;
    int readShipRoute(const string& full_path_and_file_name) override ;
    int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) override;
    int getInstructionsForCargo(
            const std::string& input_full_path_and_file_name,
            const std::string& output_full_path_and_file_name) override;
    void unloadInstructions(std::ofstream& file);
    void loadInstructions(std::ofstream& file, vector<unique_ptr<Container>>& list);
    virtual Position findPosition(const unique_ptr<Container>& container) ;
    bool rejectingContainer(unique_ptr<Container>& container);
    void unloadContainersAbove(Position position, std::ofstream& file);
    string instructionToString(char instruction, const string& id, Position pos);
    int countContainersOnPort(const string& id);
};


#endif //STOWAGE__208967075_C_H
