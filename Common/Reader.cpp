#include "Reader.h"

bool Reader::splitLine(string& line, vector<string>& vec, int n, bool warning) {
    std::stringstream ssin(line);
    int i = 0;
    string str, left;
    while (ssin.good() && i < n){
        ssin >> std::skipws >> str;
        str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char x){return std::isspace(x);}), str.end());
        if (i != n - 1) {
            if (str[str.length() - 1] != ',') {
                if (warning) {
                    std::cout << "WARNING: wrong format. This line will be ignored" << std::endl;
                }
                else { std::cout << "Bad Input: wrong format." << std::endl; }
                return false;
            }
            str.pop_back();
        }
        vec[i] = str;
        i++;
    }
    while (ssin.good()) {
        ssin >> std::skipws >> left;
        if (!std::all_of(left.begin(),left.end(),[](unsigned char x){return std::isspace(x);})) {
            i++;
        }
    }
    if(i != n) {
        if (warning) {
            std::cout << "WARNING: wrong number of arguments. This line will be ignored" << std::endl;
        }
        else { std::cout << "Bad Input: wrong number of arguments" << std::endl; }
        return false;
    }
    return true;
}

bool Reader::convertVectorToInt(vector<int>& int_vec, vector<string>& str_vec, bool warning, int sign) {
    string number_sign = "negative";
    if (sign == -1) {
        number_sign = "positive";
    }
    try {
        for (unsigned long long i = 0; i < str_vec.size(); i++) {
            int number = stoi(str_vec[i]);
            if (number*sign < 0) {
                std::cout << sign << " warn about " << (number) << std::endl;
                if (warning) {
                    std::cout << "WARNING: number can't be " << number_sign <<". This line will be ignored" << std::endl;
                }
                else { std::cout << "Bad input: number can't be " << number_sign << std::endl; }
                return false;
            }
            int_vec[i] = number;
        }
    }
    catch (std::invalid_argument const &e) {
        if (warning) {
            std::cout << "WARNING: invalid argument. This line will be ignored" << std::endl;
        }
        else { std::cout << "Bad input: invalid argument" << std::endl; }
        return false;
    }
    return true;
}

bool Reader::ignoreLine(string& str) {
    for (char c : str) {
        if (c == '#') { return true; }
        else if (!std::isspace(c)) { return false; }
    }
    return true;
}

bool Reader::splitCargoLine(string& line, string& id, int& weight, string& destination) {
    vector<string> str_vec(3);
    if(!splitLine(line, str_vec, 3)) {
        return false;
    }

    id = str_vec[0];
    try {
        weight = stoi(str_vec[1]);
        if (weight < 0) {
            std::cout << "WARNING: invalid argument for container weight. This line will be ignored" << std::endl;
            return false;
        }
    }
    catch (std::invalid_argument const &e) {
        std::cout << "WARNING: invalid argument for container weight. This line will be ignored" << std::endl;
        return false;
    }
    destination = str_vec[2];
    return true;
}

bool Reader::splitPlanLine(string& line, vector<int>& vec, bool warning) {
    vector<string> str_vec(3);
    if(!splitLine(line, str_vec, 3, warning)) {
        return false;
    }
    return convertVectorToInt(vec, str_vec, warning);
}

bool Reader::splitInstructionLine(string& line, char& op, string& id, int& floor, int& x, int& y) {
    vector<string> str_vec(5);
    if (!splitLine(line, str_vec, 5)) { return false; }
    if (str_vec[0].size() != 1) {
        std::cout << "WARNING: operation should be represented by a single char. This line will be ignored" << std::endl;
        return false;
    }
    op = str_vec[0][0];
    if (op != 'L' && op != 'U' && op != 'R') {
        std::cout << "WARNING: operation is illegal. This line will be ignored" << std::endl;
        return false;
    }
    id = str_vec[1];
    vector<int> int_vec(3);
    vector<string> sub_str_vec;
    std::copy(str_vec.begin() + 2, str_vec.end(), std::back_inserter(sub_str_vec));
    int sign = op == 'R' ? - 1 : 1;
    if (convertVectorToInt(int_vec, sub_str_vec, true, sign)) {
        floor = int_vec[0];
        x = int_vec[1];
        y = int_vec[2];
    }
    return true;
}

bool Reader::legalPortSymbol(string symbol) {
    return symbol.size() == 5 &&
           std::all_of(symbol.begin(), symbol.end(), [](unsigned char c){ return std::isupper(c); });
}

bool Reader::legalContainerId(string id) {
    return id.size() == 11 &&
           std::all_of(id.begin(), id.begin() + 3, [](unsigned char c){ return std::isupper(c); })
           && (id[3] == 'U' || id[3] == 'J' || id[3] == 'Z')
           && std::all_of(id.begin() + 4, id.end(), [](unsigned char c){ return std::isdigit(c); })
           && legalCheckDigit(id);
}

bool Reader::legalCheckDigit(string id) {
    int sum = 0, i = 0, temp;
    for(char ch : id) {
        if (i <= 3) {
            int add = static_cast<int>(floor((double) (ch - 'A' + 10) / 11));
            temp = ch - 'A' + 10 + add;
        }
        else if (i < 10){
            temp = ch - '0';
        }
        else { break; }
        sum += (int)pow(2, i) * temp;
        i++;
    }
    sum -= 11 * (int)floor((double)sum / 11);
    return sum % 10 == (id[10] - '0');
}

bool Reader::readCargoLoad(const string &path, vector<unique_ptr<Container>>& list) {
    //TODO: if weight or port don't exist, put -1/"". these are the cases to handle:
//    2^12 - containers at port: bad line format, missing or bad weight (ID rejected) - put -1
//    2^13 - containers at port: bad line format, missing or bad port dest (ID rejected) - put ""
//    2^14 - containers at port: bad line format, ID cannot be read (ignored) - put ""
//    2^16 - containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port) - return false;
    vector<unique_ptr<Container>> cargo;
    string line, destination,  id;
    int weight;
    std::ifstream file(path);
    while (std::getline(file, line)) {
        if (Reader::ignoreLine(line)) { continue; }
        if (Reader::splitCargoLine(line, id, weight, destination)) {
            unique_ptr<Container> container = make_unique<Container>(weight, destination, id);
            list.emplace_back(std::move(container));
        }
    }
    return true;
}

int Reader::readShipPlan(const string& path, ShipPlan& plan) {
    int errors = 0;
    fs::path file_path = path;
    if(path.empty() || !fs::exists(file_path)) {
        errors += (2 ^ 3);
    }
    int x, y, num_floors;
    std::string line;
    std::ifstream file(path);
    vector<int> vec(3);
    do {
        if (!std::getline(file, line)) {
            if(!(errors & 3)) errors += (2 ^ 3);
        }
    }
    while (Reader::ignoreLine(line));
    if (!Reader::splitPlanLine(line, vec, false)) { return false; }
    num_floors = vec[0];
    x = vec[1];
    y = vec[2];

    map< pair<int,int>, int > floors_plan;
    while (std::getline(file, line)) {
        if (Reader::ignoreLine(line)) { continue; }
        if(!Reader::splitPlanLine(line, vec)) { return false; }
        if (x < vec[0] || y < vec[1] || num_floors < vec[2]) {
            std::cout << "WARNING: invalid arguments. This line will be ignored" << std::endl;
            continue;
        }
        floors_plan[{vec[0], vec[1]}] = vec[2];
    }
    plan = ShipPlan(num_floors, floors_plan);
    return errors;
}

int Reader::readShipRoute(const string &path, ShipRoute& route) {
    fs::path file_path = path;
    if(path.empty() || !fs::exists(file_path)) {
        std::cout << "No file" << std::endl;
        return false;
    }
    string curr_port, prev_port;
    vector<string> ports;
    std::ifstream file(path);
    while (std::getline(file, curr_port)) {
        curr_port.erase(std::remove_if(curr_port.begin(), curr_port.end(), [](unsigned char x){return std::isspace(x);})
                , curr_port.end());
        if (Reader::ignoreLine(curr_port)) { continue; }
        if(!Reader::legalPortSymbol(curr_port)) {
            std::cout << "Bad input: port symbol is illegal" << std::endl;
        }
        if (curr_port == prev_port) {
            std::cout << "Bad input: port can not appear in two consecutive lines" << std::endl;
        }
        ports.emplace_back(curr_port);
        prev_port = curr_port;
    }
    route = ShipRoute(ports);
    return true;
}

bool Reader::checkDirPath(const string& pathName) {
    fs::path path = pathName;
    return fs::is_directory(path);
}

vector<string> Reader::getTravels(const string &dir) {
    vector<string> travels;
    std::regex format("(.*)_travel");
    for(const auto & entry : fs::directory_iterator(dir)) {
        if(std::regex_match(entry.path().stem().string(), format)) {
            travels.emplace_back(entry.path().stem().string());
        }
    }
    return travels;
}

vector<Operation> Reader::getInstructionsVector(const string &path) {
    vector<Operation> ops;
    char op_char;
    int floor, x, y;
    string line, id;
    std::ifstream file(path);
    while (std::getline(file, line)) {
        if (Reader::ignoreLine(line)) { continue; }
        if (Reader::splitInstructionLine(line, op_char, id, floor, x, y)) {
            Operation op = Operation(op_char, id, Position(floor, x, y));
            ops.push_back(op);
        }
    }
    return ops;
}