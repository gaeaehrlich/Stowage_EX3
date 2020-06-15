#include "Reader.h"

int Reader::splitLine(string& line, vector<string>& vec, int n, bool exact) {
    int i = 0;
    std::regex r("\\s*,\\s*");
    line = std::regex_replace(line, std::regex("^\\s+"), "");
    line = std::regex_replace(line, std::regex("\\s+$"), "");
    std::sregex_token_iterator s(line.begin(), line.end(), r, -1), end;
    for (;i < n && s != end; s++, i++) {
        string str = s -> str();
        vec[i] = str;
    }

    return (!exact || s == end) && (i == n);
}

bool Reader::convertVectorToInt(vector<int>& intVec, vector<string>& strVec) {
    try {
        for (unsigned long long i = 0; i < strVec.size(); i++) {
            int number = stoi(strVec[i]);
            intVec[i] = number;
        }
    }
    catch (std::invalid_argument const &e) {
        return false;
    }
    return true;
}

bool Reader::ignoreLine(string& line) {
    std::regex format("^#.*|^\\s*$");
    return std::regex_match(line, format);
}

int Reader::splitCargoLine(string& line, string& id, int& weight, string& destination) {
    vector<string> vec(3);
    int errors = 0;
    splitLine(line, vec, 3);
    id = vec[0];
    if (id == "") { errors |= pow2(14);}
    else if (!legalContainerId(id)) { errors |= pow2(15); }
    try {
        weight = stoi(vec[1]);
        if (weight < 0) { errors |= pow2(12); weight = -1; }
    }
    catch (std::invalid_argument const &e) { errors |= pow2(12); weight = -1; }
    destination = vec[2];
    std::transform(destination.begin(), destination.end(), destination.begin(),
               [](unsigned char c){ return std::toupper(c); });
    if (!legalPortSymbol(destination)) { errors |= pow2(13); destination = ""; }
    return errors;
}

bool Reader::splitPlanLine(string& line, vector<int>& vec) {
    vector<string> str_vec(3);
    if(!splitLine(line, str_vec, 3)) {
        return false;
    }
    return convertVectorToInt(vec, str_vec) && vec[0] >= 0 && vec[1] >= 0 && vec[2] >= 0;
}

bool Reader::splitInstructionLine(string& line, char& op, string& id, Position& position, Position& move) {
    bool rejecting = std::regex_match(line, std::regex("^\\s*R.*"));
    bool moving = std::regex_match(line, std::regex("^\\s*M.*"));
    int n = moving ? 8 : (rejecting ? 2 : 5);
    vector<string> strVec(n);
    if (!splitLine(line, strVec, n, !rejecting)) { return false; }
    if (strVec[0] != "L" && strVec[0] != "U" && strVec[0] != "R" && strVec[0] != "M") { return false; }
    op = strVec[0][0];
    id = strVec[1];
    if (rejecting) { return true; }
    vector<int> intVec(n - 2);
    vector<string> subStrVec;
    std::copy(strVec.begin() + 2, strVec.end(), std::back_inserter(subStrVec));
    if (!convertVectorToInt(intVec, subStrVec)) { return false; }
    position = Position(intVec[0], intVec[1], intVec[2]);
    if (moving) {
        move = Position(intVec[3], intVec[4], intVec[5]);
    }
    return true;
}

bool Reader::legalPortSymbol(const string& symbol) {
    std::regex format("^[A-Z]{5}$");
    return std::regex_match(symbol, format);
}

bool Reader::legalContainerId(const string& id) {
    std::regex format("^[A-Z]{3}[UJZ][0-9]{7}$");
    return std::regex_match(id, format) && legalCheckDigit(id);
}

bool Reader::legalCheckDigit(const string& id) {
    int sum = 0, i = 0, temp, p = 1;
    int val[26] = {10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 23, 24,
                   25, 26, 27, 28, 29, 30, 31, 32, 34, 35, 36, 37, 38};
    for(char ch : id) {
        if (i <= 3) {
            temp = val[ch - 'A'];
        }
        else if (i < 10){
            temp = ch - '0';
        }
        else { break; }
        sum += p* temp;
        p *= 2;
        i++;
    }
    sum -= 11 * (int)floor((double)sum / 11);
    return sum % 10 == (id[10] - '0');
}

int Reader::readCargoLoad(const string &path, vector<unique_ptr<Container>>& list) {
    vector<unique_ptr<Container>> cargo;
    std::string line, destination,  id;
    int weight, errors = 0;
    fs::path filePath = path;
    if(path.empty() || !fs::exists(filePath)) { return  pow2(16); }
    std::ifstream file(path);
    while (std::getline(file, line)) {
        if (ignoreLine(line)) { continue; }
        errors |= splitCargoLine(line, id, weight, destination);
        unique_ptr<Container> container = make_unique<Container>(weight, destination, id);
        list.emplace_back(std::move(container));
    }
    return errors;
}

int Reader::readShipPlan(const string& path, ShipPlan& plan) {
    int errors = 0, x, x1, y, y1, numFloors, numFloors1;
    fs::path filePath = path;
    if(path.empty() || !fs::exists(filePath)) { return  pow2(3); }
    std::string line; std::ifstream file(path);
    if (!file || file.peek() == std::ifstream::traits_type::eof()) { return pow2(3); }
    vector<int> vec(3);
    do {
        if (!std::getline(file, line)) { return pow2(3); }
    }
    while (ignoreLine(line));
    if (!Reader::splitPlanLine(line, vec)) { return pow2(3); }
    numFloors = vec[0]; x = vec[1]; y = vec[2];
    bool fatal = false;
    map< pair<int,int>, int > mPlan;
    while (std::getline(file, line)) {
        if (ignoreLine(line)) { continue; }
        if(!Reader::splitPlanLine(line, vec)) { // wrong format
            errors |= pow2(2);
            continue;
        }
        x1 = vec[0]; y1 = vec[1]; numFloors1 = vec[2];
        if (numFloors <= numFloors1) { // floors
            errors |= pow2(0);
            continue;
        }
        if (x <= x1 || y <= y1) { // wrong values
            errors |= pow2(1);
            continue;
        }
        if (mPlan.find({x1, y1}) != mPlan.end()) { // duplicate x,y appearance
            if (mPlan[{x1, y1}] == numFloors1) { // same data
                errors |= pow2(2);
            }
            else { // different data
                errors |= pow2(4);
                fatal = true;
            }
            continue;
        }
        mPlan[{x1, y1}] = numFloors1;
    }
    if (!fatal) {
        for (int i = 0; i < x; i++) {
            for (int j = 0; j < y; j++) {
                if (mPlan.find({i, j}) == mPlan.end()) {
                    mPlan[{i, j}] = numFloors;
                }
            }
        }
        plan = ShipPlan(numFloors, std::move(mPlan));
    }
    return errors;
}

int Reader::readShipRoute(const string &path, ShipRoute& route) {
    fs::path filePath = path;
    if(path.empty() || !fs::exists(filePath)) {
        return pow2(7);
    }
    int errors = 0;
    string currPort, prevPort;
    vector<string> ports;
    std::ifstream file(path);
    if (!file || file.peek() == std::ifstream::traits_type::eof()) { return pow2(7); }
    while (std::getline(file, currPort)) {
        if (ignoreLine(currPort)) { continue; }
        currPort = std::regex_replace(currPort, std::regex("^\\s+"), "");
        currPort = std::regex_replace(currPort, std::regex("\\s+$"), "");
        std::transform(currPort.begin(), currPort.end(), currPort.begin(),
                       [](unsigned char c){ return std::toupper(c); });
        if (currPort == prevPort) {
            errors |= pow2(5);
            continue;
        }
        if(!Reader::legalPortSymbol(currPort)) {
            errors |= pow2(6);
            continue;
        }
        ports.emplace_back(currPort);
        prevPort = currPort;
    }
    if (ports.size() == 1) { errors |= pow2(8); }
    else { route = ShipRoute(ports); }
    return errors;
}

bool Reader::checkDirPath(const string& pathName) {
    fs::path path = pathName;
    return fs::is_directory(path);
}

vector<string> Reader::getTravels(const string &dir) {
    vector<string> travels;
    for(const auto & entry : fs::directory_iterator(dir)) {
        if(fs::is_directory(entry.path())) {
            travels.emplace_back(entry.path().stem().string());
        }
    }
    return travels;
}

vector<Operation> Reader::getInstructionsVector(const string &path) {
    vector<Operation> ops;
    char opChar;
    Position position, move;
    string line, id;
    std::ifstream file(path);
    while (std::getline(file, line)) {
        if (Reader::ignoreLine(line)) { continue; }
        if (Reader::splitInstructionLine(line, opChar, id, position, move)) {
            ops.emplace_back(opChar, id, position, move);
        }
        else {
            ops.emplace_back(ERROR, "", Position());
        }
    }
    return ops;
}
