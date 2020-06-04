#ifndef STOWAGE_READER_H
#define STOWAGE_READER_H

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <tuple>
#include <memory>
#include <map>
#include <experimental/filesystem>
#include <fstream>
#include <regex>
#include <cmath>

#include "Container.h"
#include "ShipPlan.h"
#include "ShipRoute.h"
#include "Operation.h"

using std::vector;
using std::string;
using std::map;
using std::unique_ptr;
using std::make_unique;
namespace fs = std::experimental::filesystem;

#define pow2(x) (int)pow(2, x)

class Reader {
public:
    static bool splitInstructionLine(string& line, char& op, string& id, Position& position, Position& move);
    static bool splitPlanLine(string& line, vector<int>& vec);
    static int splitCargoLine(string& line, string& id, int& weight, string& destination);
    static int splitLine(string& line, vector<string>& vec, int n, bool exact = true);
    static bool convertVectorToInt(vector<int>& intVec, vector<string>& strVec);
    static bool ignoreLine(string& str);
    static bool legalPortSymbol(const string& symbol);
    static bool legalContainerId(const string& id);
    static bool legalCheckDigit(const string& id);
    static int readCargoLoad(const string& path, vector<unique_ptr<Container>>& list);
    static int readShipPlan(const string& path, ShipPlan& plan);
    static int readShipRoute(const string& path, ShipRoute& route);
    static bool checkDirPath(const string& pathName);
    static vector<string> getTravels(const string& dir);
    static vector<Operation> getInstructionsVector(const string &path);
};


#endif //STOWAGE_READER_H
