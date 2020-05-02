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
#include <filesystem>
#include <fstream>
#include <regex>


#include "Container.h"
#include "ShipPlan.h"
#include "ShipRoute.h"

using std::vector;
using std::string;
using std::map;
using std::unique_ptr;
namespace fs = std::filesystem;


class Reader {
public:
    static bool splitInstructionLine(string& line, char& op, string& id, int& floor, int& x, int& y);
    static bool splitPlanLine(string& line, vector<int>& vec, bool warning = true);
    static bool splitCargoLine(string& line, string& id, int& weight, string& destination);
    static bool splitLine(string& line, vector<string>& vec, int n, bool warning = true);
    static bool convertVectorToInt(vector<int>& int_vec, vector<string>& str_vec, bool warning = true, int sign = 1);
    static bool ignoreLine(string& str);
    static bool legalPortSymbol(string symbol);
    static bool legalContainerId(string id);
    static bool legalCheckDigit(string id);
    static bool readCargoLoad(const string& path, vector<unique_ptr<Container>>& list);
    static bool readShipPlan(const string& path, ShipPlan& plan);
    static bool readShipRoute(const string& path, ShipRoute& route);
    static bool checkDirPath(const string& pathName);
    static int getTravels(const string& dir);
    static string getPath(const string& dir, const string& search);
    static map<string, vector< pair<int, string>>> getCargoPath(const string& dir);
};


#endif //STOWAGE_READER_H
