#ifndef STOWAGE__208967075_C_H
#define STOWAGE__208967075_C_H


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "../common/Container.h"
#include "../common/ShipPlan.h"
#include "../common/ShipRoute.h"
#include "../common/Reader.h"
#include "../common/WeightBalanceCalculator.h"
#include "../common/Reader.h"
#include "../algorithm/AbstractAlgorithm.h"
#include "../algorithm/AlgorithmRegistration.h"


using std::unique_ptr;
using std::string;
using std::vector;

class _208967075_c: public AbstractAlgorithm {
    vector<unique_ptr<Container>> _cargoLoad;

protected:
    WeightBalanceCalculator _calc;
    ShipRoute _route;
    ShipPlan _plan;

public:
    ~_208967075_c() override;
    void finishedPort();
    int readCargoLoad(const string& path);
    int readShipPlan(const string& path) override ;
    int readShipRoute(const string& path) override ;
    int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) override;
    int getInstructionsForCargo(
            const std::string& inputPath,
            const std::string& outputPath) override;
    void unloadInstructions(std::ofstream& file);
    void loadInstructions(std::ofstream& file, vector<unique_ptr<Container>>& list);
    virtual Position findPosition(const unique_ptr<Container>& container) ;
    bool rejectingContainer(unique_ptr<Container>& container);
    void unloadContainersAbove(Position position, std::ofstream& file);
    string instructionToString(char instruction, const string& id, Position pos);
};


#endif //STOWAGE__208967075_C_H
