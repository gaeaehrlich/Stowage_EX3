#ifndef STOWAGE_NAIVEALGORITHM_H
#define STOWAGE_NAIVEALGORITHM_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include "../common/Container.h"
#include "../common/ShipPlan.h"
#include "../common/ShipRoute.h"
#include "../common/Reader.h"
#include "../common/WeightBalanceCalculator.h"
#include "../common/Reader.h"
#include "AbstractAlgorithm.h"
#include "AlgorithmRegistration.h"


using std::unique_ptr;
using std::string;
using std::vector;

class NaiveAlgorithm: public AbstractAlgorithm {
    vector<unique_ptr<Container>> _cargoLoad;
    vector<unique_ptr<Container>> _temporaryUnloaded;
    bool _invalidTravel = false;
    int _status = 0;

protected:
    WeightBalanceCalculator _calc;
    ShipRoute _route;
    ShipPlan _plan;

public:
    ~NaiveAlgorithm() override;
    void finishedPort();
    int readCargoLoad(const string& full_path_and_file_name);
    int readShipPlan(const string& path) override ;
    int readShipRoute(const string& path) override ;
    int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) override;
    int getInstructionsForCargo(
            const std::string& inputPath,
            const std::string& outputPath) override;
    void sortCargoLoad();
    void unloadInstructions(std::ofstream& file);
    void loadInstructions(std::ofstream& file, vector<unique_ptr<Container>>& list);
    virtual Position findPosition(const unique_ptr<Container>& container) = 0;
    bool shouldRejectContainer(unique_ptr<Container>& container);
    void unloadContainersAbove(Position position, std::ofstream& file);
    string instructionToString(char instruction, const string& id, Position pos);
    int countContainersOnPort(const string& id);
    void rejectInstructions(std::ofstream& file);
    void removeDuplicates(std::ofstream &file);
};


#endif //STOWAGE_NAIVEALGORITHM_H
