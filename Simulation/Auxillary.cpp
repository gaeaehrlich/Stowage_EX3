#include "Simulation.h"

string Simulation::getPath(const string& travel_dir, const string& search) {
    std::regex format("(.*)\\." + search);
    for(const auto & entry : fs::directory_iterator(travel_dir)) {
        if(std::regex_match(entry.path().string(), format)) {
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

void Simulation::writeReaderErrors(std::ofstream& file, int simulation_errors, int alg_errors, vector<string> error_msg, const string& alg_name, int index) {
    for(long unsigned int i = 0; i < error_msg.size(); i++) {
        if(simulation_errors & pow2(i + index)) {
            file << "INPUT FILE ERROR: " << error_msg[i];
            if(!(alg_errors & pow2(i + index))) {
                file << "ALGORITHM WARNING: algorithm did not alert this problem\n";
            }
        }
        else if(alg_errors & pow2(i + index)) {
            file << "ALGORITHM WARNING: algorithm " << alg_name << " reports a problem the simulation did not find: " << error_msg[i];
        }
    }
}


bool Simulation::writeShipErrors(const string &error_path, int simulation_errors, int alg_errors, const string& travel, const string& alg_name) {
    if(simulation_errors == 0 && alg_errors == 0) {
        return true;
    }
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    file << "---------------------------------------------------------------------\n";
    file << "***** ALGORITHM: " << alg_name << ", TRAVEL: " << travel << " *****\n";
    vector<string> error_msg;
    error_msg.emplace_back("ship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n");
    error_msg.emplace_back("ship plan: a given position exceeds the X/Y ship limits (ignored)\n");
    error_msg.emplace_back("ship plan: bad line format after first line (ignored)\n");
    error_msg.emplace_back("ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n");
    error_msg.emplace_back("ship plan: travel error - duplicate x,y appearance with different data (cannot run this travel)\n");
    error_msg.emplace_back("travel route: a port appears twice or more consecutively (ignored)\n");
    error_msg.emplace_back("travel route: bad port symbol format (ignored)\n");
    error_msg.emplace_back("travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n");
    error_msg.emplace_back("travel route: travel error - file with only a single valid port (cannot run this travel)\n");
    writeReaderErrors(file, simulation_errors, alg_errors, error_msg, alg_name);
    file.close();
    return simulation_errors & pow2(3) || simulation_errors & pow2(4) || simulation_errors & pow2(7) || simulation_errors & pow2(8);
}


int Simulation::countContainersOnPort(const string& id, vector<unique_ptr<Container>>& containersAtPort) {
    int count = 0;
    for(auto& container: containersAtPort) {
        if(container != nullptr && container -> getId() == id) {
            count++;
        }
    }
    return count;
}


void Simulation::writeCargoErrors(const string &error_path, int simulation_errors, int alg_errors, vector<unique_ptr<Container>>& containersAtPort, const string& travel_name, const string& alg_name) {
    if(simulation_errors == 0 && alg_errors == 0) {
        return;
    }
    std::ofstream file;
    file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    file << "---------------------------------------------------------------------\n";
    file << "***** ALGORITHM: " << alg_name << ", TRAVEL: " << travel_name << ", PORT: " << _route.getCurrentPort() <<" *****\n";
    vector<string> error_msg;
    error_msg.emplace_back("containers at port: duplicate ID on port (ID rejected)\n");
    error_msg.emplace_back("containers at port: ID already on ship (ID rejected)\n");
    error_msg.emplace_back("containers at port: bad line format, missing or bad weight (ID rejected)\n");
    error_msg.emplace_back("containers at port: bad line format, missing or bad port dest (ID rejected)\n");
    error_msg.emplace_back("containers at port: bad line format, ID cannot be read (ignored)\n");
    error_msg.emplace_back("containers at port: illegal ID check ISO 6346 (ID rejected)\n");
    error_msg.emplace_back("containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n");
    error_msg.emplace_back("containers at port: last port has waiting containers (ignored)\n");
    error_msg.emplace_back("containers at port: total containers amount exceeds ship capacity (rejecting far containers)\n");
    for(const auto& container: containersAtPort) {
        if(countContainersOnPort(container -> getId(), containersAtPort) > 1) { // 2^10
            if(!simulation_errors & pow2(10)) { error_msg[0].append("The duplicated containers: "); }
            simulation_errors |= pow2(10);
            error_msg[0].append(container -> getId());
        }
        if(_plan.hasContainer(container -> getId())) { // 2^11
            if(!simulation_errors & pow2(11)) { error_msg[1].append("The container: "); }
            simulation_errors |= pow2(11);
            error_msg[1].append(container -> getId());
            file << "containers at port: ID already on ship (ID rejected). ID number: " << container -> getId() << "\n";
        }
    }
    if(simulation_errors & pow2(10 )) { error_msg[0].append("\n"); }
    if(simulation_errors & pow2(11 )) { error_msg[1].append("\n"); }
    if(_route.isLastStop() && !containersAtPort.empty()) { simulation_errors |= pow2(17); }
    if(_plan.numberOfEmptyCells() + _plan.findContainersToUnload(_route.getCurrentPort()).size() < containersAtPort.size()) { simulation_errors |= pow2(18); }
    writeReaderErrors(file, simulation_errors, alg_errors, error_msg, alg_name, 10);
    file.close();
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

void Simulation::writeResults(const string &path, const map<string, vector<int>>& results) {
    vector<tuple<string, vector<int>, int, int>> res_vec;
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
        if(std::regex_match(entry.path().string(), format) && files.find((entry.path().stem().string().append(".cargo_data"))) == files.end()) {
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << "WARNING: the travel folder "<< curr_travel_path << " has a cargo_data file that is not in use: " << entry.path().string() << "\n";
            file.close();
        }
    }
}