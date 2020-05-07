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
    std::filesystem::path path = pathName;
    if(!std::filesystem::exists(path)) { // if cargo file doesn't exist, create an empty one
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
    if(errors & 0) {
        file << "ship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n";
    }
    if(errors & 1) {
        file << "ship plan: a given position exceeds the X/Y ship limits (ignored)\n";
    }
    if(errors & 2) {
        file << "ship plan: bad line format after first line (ignored)\n";
    }
    if(errors & 3) {
        file << "ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n";
        fatal = true;
    }
    if(errors & 4) {
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
    if(errors & 5) {
        file << "travel route: a port appears twice or more consecutively (ignored)\n";
    }
    if(errors & 6) {
        file << "travel route: bad port symbol format (ignored)\n";
    }
    if(errors & 7) {
        file << "travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n";
        fatal = true;
    }
    if(errors & 8) {
        file << "travel route: travel error - file with only a single valid port (cannot run this travel)\n";
        fatal = true;
    }
    file.close();
    return !fatal;
}

bool Simulation::writeCargoErrors(const string &error_path, int errors) {
    if(errors == 0){
        return true;
    }
    bool fatal = false;
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    if(errors & 10) { //countContainersOnPort(container ->getId()) > 0
        file << "containers at port: duplicate ID on port (ID rejected)\n";
    }
    if(errors & 11) { //_plan.hasContainer(container ->getId())
        file << "containers at port: ID already on ship (ID rejected)\n";
    }
    if(errors & 12) { //container -> getWeight() < 0
        file << "containers at port: bad line format, missing or bad weight (ID rejected)\n";
    }
    if(errors & 13) { // !Reader::legalPortSymbol(container -> getDest()) || !_route.portInRoute(container -> getDest()) || (container-> getDest() == _route.getCurrentPort())
        file << "containers at port: bad line format, missing or bad port dest (ID rejected)\n";
    }
    if(errors & 14) { // container -> getId().empty()
        file << "containers at port: bad line format, ID cannot be read (ignored)\n";
    }
    if(errors & 15) { // !Reader::legalContainerId(container ->getId())
        file << "containers at port: illegal ID check ISO 6346 (ID rejected)\n";
    }
    if(errors & 16) {
        file << "containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n";
    }
    if(errors & 17) { // _route.isLastStop()
        file << "containers at port: last port has waiting containers (ignored)\n";
    }
    if(errors & 18) { // _plan.isFull()
        file << "containers at port: total containers amount exceeds ship capacity (rejecting far containers)\n";
    }
    file.close();
    return !fatal;
}


void Simulation::writeResults(const string &path, vector<tuple<string, vector<int>, int, int>> results, const vector<string>& travels) {
    sort(results.begin(), results.end(), []( const auto& res1, const auto& res2 ) {
        if(std::get<3>(res1) == std::get<3>(res2)) { // if same #errors sort by #operations
            return std::get<2>(res1) < std::get<2>(res2);
        }
        return std::get<3>(res1) < std::get<3>(res2);
    });
    std::ofstream file(path);
    file << "RESULTS,";
    for(const string& travel: travels) {
        file << travel << ",";
    }
    file << "Sum,\nNum Errors\n";
    for(auto& alg_result: results) {
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