#include "Simulation.h"

string Simulation::getPath(const string& travel_dir, const string& search) {
    std::regex format("(.*)\\." + search);
    for(const auto & entry : fs::directory_iterator(travel_dir)) {
        if(std::regex_match(entry.path().stem().string(), format)) {
            return entry.path().string();
        }
    }
    return "";
}

string Simulation::getCargoPath(const string &travel_dir, const string &port) {
    string pathName = travel_dir + SUBDIR + port + "_" + std::to_string(_route.getPortNumber()) + ".cargo_data";
    fs::path path = pathName;
    if(!fs::exists(path)) { // if cargo file doesn't exist, create an empty one
        std::ofstream cargo_file (pathName);
        cargo_file.close();
    }
    return pathName;
}

string Simulation::createPortOutputFile(const string &output_path, const string &port) {
    string file = output_path + SUBDIR + port + "_" + std::to_string(_route.getPortNumber()) + ".crane_instructions";
    std::ofstream outfile(file);
    outfile.close();
    return file;
}

string Simulation::createTravelOutputFolder(const string &output_path, const string &alg_name, const string &travel_name) {
    string path = output_path + SUBDIR + alg_name + "_" + travel_name + "_crane_instructions";
    fs::create_directory(path);
    return path;
}

bool Simulation::checkDirectories(const string &travel_path, const string &algorithm_path, const string &output_path) {
    bool travel = Reader::checkDirPath(travel_path);
    bool algorithm = Reader::checkDirPath(algorithm_path);
    bool output = Reader::checkDirPath(output_path);
    if(!travel) {
        std::cout << "The path for the travels is incorrect: " << travel_path << std::endl;
    }
    if(!algorithm) {
        std::cout << "The path for the algorithms is incorrect: " << algorithm_path << std::endl;
    }
    if(!output) {
        std::cout << "The path for the output is incorrect: " << output_path << std::endl;
    }
    return travel && algorithm && output;
}

bool Simulation::writeShipPlanErrors(const string &error_path, int errors, const string& travel) {
    if(errors == 0) {
        return true;
    }
    bool fatal = false;
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    file << "SHIP PLAN ERRORS FOR TRAVEL: " << travel << "\n";
    if(errors & (2^0)) {
        file << "ship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n";
    }
    if(errors & (2^1)) {
        file << "ship plan: a given position exceeds the X/Y ship limits (ignored)\n";
    }
    if(errors & (2^2)) {
        file << "ship plan: bad line format after first line (ignored)\n";
    }
    if(errors & (2^3)) {
        file << "ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n";
        fatal = true;
    }
    if(errors & (2^4)) {
        file << "ship plan: travel error - duplicate x,y appearance with different data (cannot run this travel)";
        fatal = true;
    }
    file.close();
    return !fatal;
}

bool Simulation::writeShipRouteErrors(const string &error_path, int errors, const string& travel) {
    if(errors == 0) {
        return true;
    }
    bool fatal = false;
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    file << "SHIP PLAN ERRORS FOR TRAVEL: " << travel << "\n";
    if(errors & (2^5)) {
        file << "travel route: a port appears twice or more consecutively (ignored)\n";
    }
    if(errors & (2^6)) {
        file << "travel route: bad port symbol format (ignored)\n";
    }
    if(errors & (2^7)) {
        file << "travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n";
        fatal = true;
    }
    if(errors & (2^8)) {
        file << "travel route: travel error - file with only a single valid port (cannot run this travel)\n";
        fatal = true;
    }
    file.close();
    return !fatal;
}

bool Simulation::writeCargoErrors(const string &error_path, int errors) { // todo: what
    if(errors == 0) {
        return true;
    }
    bool fatal = false;
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    if(errors & (2^10)) { //countContainersOnPort(container ->getId()) > 0
        file << "containers at port: duplicate ID on port (ID rejected)\n";
    }
    if(errors & (2^11)) { //_plan.hasContainer(container ->getId())
        file << "containers at port: ID already on ship (ID rejected)\n";
    }
    if(errors & (2^12)) { // container -> getWeight() <= 0
        file << "containers at port: bad line format, missing or bad weight (ID rejected)\n";
    }
    if(errors & (2^13)) { // !Reader::legalPortSymbol(container -> getDest())
        file << "containers at port: bad line format, missing or bad port dest (ID rejected)\n";
    }
    if(errors & (2^14)) { // container -> getId().empty()
        file << "containers at port: bad line format, ID cannot be read (ignored)\n";
    }
    if(errors & (2^15)) { // !Reader::legalContainerId(container ->getId())
        file << "containers at port: illegal ID check ISO 6346 (ID rejected)\n";
    }
    if(errors & (2^16)) {
        file << "containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n";
    }
    if(errors & (2^17)) { // _route.isLastStop()
        file << "containers at port: last port has waiting containers (ignored)\n";
    }
    if(errors & (2^18)) { // _plan.isFull()
        file << "containers at port: total containers amount exceeds ship capacity (rejecting far containers)\n";
    }
    file.close();
    return !fatal;
}


string Simulation::createResultsFile(const string &output_path, vector<string> travels) {
    string results_path = output_path + SUBDIR + "simulation.results";
    std::ofstream file(results_path);
    file << "RESULTS, ";
    for(const string& travel: travels) {
        file << travel << ", ";
    }
    file << "Sum, Num Errors\n";
    file.close();
    return results_path;
}

void Simulation::writeResults(const string &path, const map<string, vector<int>>& results, const vector<string>& travels) {
    vector<tuple<string, vector<int>, int, int>> res_vec(results.size());
    std::ofstream file;
    file.open(path, std::ios_base::app);
    for(auto& alg_res: results) {
        res_vec.emplace_back(alg_res.first, alg_res.second, sumResults(alg_res.second, true), sumResults(alg_res.second, false));
    }
    sort(res_vec.begin(), res_vec.end(), []( const auto& res1, const auto& res2 ) {
        if(std::get<3>(res1) == std::get<3>(res2)) { // if same #errors sort by #operations
            return std::get<2>(res1) < std::get<2>(res2);
        }
        return std::get<3>(res1) < std::get<3>(res2);
    });
    for(auto& alg_result: res_vec) {
        file << std::get<0>(alg_result) << ","; // algorithm name
        for(auto& travel_result: std::get<1>(alg_result)) { // iterating over each travel result
            file << travel_result << ",";
        }
        file << std::get<2>(alg_result) << "," << std::get<3>(alg_result) << "\n";
    }
}

int Simulation::sumResults(const vector<int>& results, bool sumOrErr) {
    int count = 0;
    for(auto& result: results) {
        if(sumOrErr && result != -1) count += result;
        if(!sumOrErr && result == -1) count++;
    }
    return count;
}


void Simulation::scanTravelPath(const string &curr_travel_path, const string &error_path) {
    map<string, int> occurrence;
    std::unordered_set<string> files;
    for(const string& port: _route.getRoute()) {
        if(occurrence.count(port) == 0) {
            occurrence[port] = 0;
        }
        occurrence[port]++;
        files.insert(port + "_" + std::to_string(occurrence[port]) + ".cargo_data");
    }
    std::regex format("(.*)\\.cargo_data");
    std::ofstream file;
    for(const auto & entry : fs::directory_iterator(curr_travel_path)) {
        if(std::regex_match(entry.path().stem().string(), format) && files.find(entry.path().stem().string()) == files.end()) {
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << "WARNING: the travel folder "<< curr_travel_path << " has a cargo_data file that is not in use: " << entry.path().string() << "\n";
        }
    }
}