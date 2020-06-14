#ifndef STOWAGE_BASEALGORITHM_H
#define STOWAGE_BASEALGORITHM_H

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

class BaseAlgorithm: public AbstractAlgorithm {
    vector<unique_ptr<Container>> _cargoLoad;
    vector<unique_ptr<Container>> _temporaryUnloaded;
    map<pair<int, int>, int> _countContainersToUnload;
    bool _invalidTravel = false;
    int _status = 0;

protected:
    WeightBalanceCalculator _calc;
    ShipRoute _route;
    ShipPlan _plan;

public:
    ~BaseAlgorithm() override;
    void finishedPort();
    int readCargoLoad(const string& full_path_and_file_name);
    int readShipPlan(const string& path) override ;
    int readShipRoute(const string& path) override ;
    int setWeightBalanceCalculator(WeightBalanceCalculator& calculator) override;
    int getInstructionsForCargo(
            const std::string& inputPath,
            const std::string& outputPath) override;
    void sortCargoLoad();
    void countSortCargo(vector<unique_ptr<Container>>& cargo, bool reverse = false);
    void buildUnloadPositions(const vector<Position>& unload);
    void unloadInstructions(std::ofstream& file);
    void loadInstructions(std::ofstream& file, vector<unique_ptr<Container>>& list);
    Position findPosition(const unique_ptr<Container>& container);
    // @param oldLoc - the previous location of the container. oldLoc != {-1, -1} when called by "tryMoveFrom"
    // if ordered = true, returns position s.t container belows port is after our given container
    // [if there is no such position, returns bad position: Position() = Position(-1,-1,-1) ]
    virtual bool tryMoveFrom(unique_ptr<Container>& container, const Position old, std::ofstream &file) = 0;
    bool shouldRejectContainer(unique_ptr<Container>& container);
    void unloadContainersAbove(Position position, std::ofstream& file);
    string instructionToString(char instruction, const string& id, const Position pos, const Position move = Position());
    int countContainersOnPort(const string& id);
    void rejectInstructions(std::ofstream& file);
    void removeDuplicates(std::ofstream &file);
};


#endif //STOWAGE_BASEALGORITHM_H
