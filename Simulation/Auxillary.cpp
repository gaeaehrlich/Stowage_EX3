#include "Simulation.h"

string Simulation::getPath(const string& travelDir, const string& search) {
    std::regex format("(.*)\\." + search);
    for(const auto & entry : fs::directory_iterator(travelDir)) {
        if(std::regex_match(entry.path().string(), format)) {
            return entry.path().string();
        }
    }
    return "";
}

string Simulation::getCargoPath(const string &travelDir, const string &port) {
    string pathName = travelDir + SUBDIR + port + "_" + std::to_string(_route.getPortNumber()) + ".cargo_data";
    fs::path path = pathName;
    if(!fs::exists(path)) { // if cargo file doesn't exist, create an empty one
        std::ofstream cargoFile (pathName);
        cargoFile.close();
    }
    return pathName;
}

string Simulation::createPortOutputFile(const string &outputPath, const string &port) {
    string file = outputPath + SUBDIR + port + "_" + std::to_string(_route.getPortNumber()) + ".crane_instructions";
    std::ofstream outfile(file);
    outfile.close();
    return file;
}

string Simulation::createTravelOutputFolder(const string &outputPath, const string &algName, const string &travelName) {
    string path = outputPath + SUBDIR + algName + "_" + travelName + "_crane_instructions";
    fs::create_directory(path);
    return path;
}

bool Simulation::checkDirectories(const string &travelPath, const string &algorithmPath, const string &outputPath) {
    bool travel = Reader::checkDirPath(travelPath);
    bool algorithm = Reader::checkDirPath(algorithmPath);
    bool output = Reader::checkDirPath(outputPath);
    if(!travel) {
        std::cout << "The path for the travels is incorrect: " << travelPath << std::endl;
    }
    if(!algorithm) {
        std::cout << "The path for the algorithms is incorrect: " << algorithmPath << std::endl;
    }
    if(!output) {
        std::cout << "The path for the output is incorrect: " << outputPath << std::endl;
    }
    return travel && algorithm && output;
}

void Simulation::writeReaderErrors(const string& errorPath, int simulationErrors, int algErrors, vector<string> errorMsg, const string& algName, const string& sailInfo, int index) {
    bool errors = false;
    string msg = sailInfo;
    for(long unsigned int i = 0; i < errorMsg.size(); i++) {
        if(simulationErrors & pow2(i + index)) {
            msg.append("INPUT FILE ERROR: " + errorMsg[i]);
            if(!(algErrors & pow2(i + index))) {
                msg.append("ALGORITHM WARNING: algorithm did not alert this problem\n");
            }
            errors = true;
        }
        else if(algErrors & pow2(i + index)) {
            msg.append("ALGORITHM WARNING: algorithm " + algName + " reports a problem the simulation did not find: " + errorMsg[i]);
            errors = true;
        }
    }
    if(errors) {
        std::ofstream file;
        file.open(errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
        file << msg;
        file.close();
    }
}


bool Simulation::writeShipErrors(const string &errorPath, int simulationErrors, int algErrors, const string& travel, const string& algName) {
    if(simulationErrors == 0 && algErrors == 0) {
        return true;
    }
    const string& sailInfo = "---------------------------------------------------------------------\n***** ALGORITHM: " + algName + ", TRAVEL: " + travel + " *****\n";
    vector<string> errorMsg;
    errorMsg.emplace_back("ship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n");
    errorMsg.emplace_back("ship plan: a given position exceeds the X/Y ship limits (ignored)\n");
    errorMsg.emplace_back("ship plan: bad line format after first line (ignored)\n");
    errorMsg.emplace_back("ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n");
    errorMsg.emplace_back("ship plan: travel error - duplicate x,y appearance with different data (cannot run this travel)\n");
    errorMsg.emplace_back("travel route: a port appears twice or more consecutively (ignored)\n");
    errorMsg.emplace_back("travel route: bad port symbol format (ignored)\n");
    errorMsg.emplace_back("travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n");
    errorMsg.emplace_back("travel route: travel error - file with only a single valid port (cannot run this travel)\n");
    writeReaderErrors(errorPath, simulationErrors, algErrors, errorMsg, algName, sailInfo);
    return !((simulationErrors & pow2(3)) || (simulationErrors & pow2(4)) || (simulationErrors & pow2(7)) || (simulationErrors & pow2(8)));
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


void Simulation::writeCargoErrors(const string &errorPath, int simulationErrors, int algErrors, vector<unique_ptr<Container>>& containersAtPort, const string& travelName, const string& algName) {
    const string& sailInfo = "---------------------------------------------------------------------\n***** ALGORITHM: " + algName + ", TRAVEL: " + travelName + " *****\n";
    vector<string> errorMsg;
    errorMsg.emplace_back("containers at port: duplicate ID on port (ID rejected)\n");
    errorMsg.emplace_back("containers at port: ID already on ship (ID rejected)\n");
    errorMsg.emplace_back("containers at port: bad line format, missing or bad weight (ID rejected)\n");
    errorMsg.emplace_back("containers at port: bad line format, missing or bad port dest (ID rejected)\n");
    errorMsg.emplace_back("containers at port: bad line format, ID cannot be read (ignored)\n");
    errorMsg.emplace_back("containers at port: illegal ID check ISO 6346 (ID rejected)\n");
    errorMsg.emplace_back("containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n");
    errorMsg.emplace_back("containers at port: last port has waiting containers (ignored)\n");
    errorMsg.emplace_back("containers at port: total containers amount exceeds ship capacity (rejecting far containers)\n");
    for(const auto& container: containersAtPort) {
        if(countContainersOnPort(container -> getId(), containersAtPort) > 1) { // 2^10
            if(!(simulationErrors & pow2(10))) { errorMsg[0].append("The duplicated containers: "); }
            simulationErrors |= pow2(10);
            errorMsg[0].append(container -> getId());
        }
        if(_plan.hasContainer(container -> getId())) { // 2^11
            if(!(simulationErrors & pow2(11))) { errorMsg[1].append("The container: "); }
            simulationErrors |= pow2(11);
            errorMsg[1].append(container -> getId());
        }
    }
    if(simulationErrors & pow2(10 )) { errorMsg[0].append("\n"); }
    if(simulationErrors & pow2(11 )) { errorMsg[1].append("\n"); }
    if(_route.isLastStop() && !containersAtPort.empty()) { simulationErrors |= pow2(17); }
    if(_plan.numberOfEmptyCells() + _plan.findContainersToUnload(_route.getCurrentPort()).size() < containersAtPort.size()) { simulationErrors |= pow2(18); }
    writeReaderErrors(errorPath, simulationErrors, algErrors, errorMsg, algName, sailInfo, 10);
}


string Simulation::createResultsFile(const string &outputPath, vector<string> travels) {
    string resultsPath = outputPath + SUBDIR + "simulation.results";
    std::ofstream file(resultsPath);
    file << "RESULTS, ";
    for(const string& travel: travels) {
        file << travel << ", ";
    }
    file << "Sum, Num Errors\n";
    file.close();
    return resultsPath;
}

void Simulation::writeResults(const string &path, const map<string, vector<int>>& results) {
    vector<tuple<string, vector<int>, int, int>> resVec;
    std::ofstream file;
    file.open(path, std::ios_base::app);
    for(auto& algRes: results) {
        resVec.emplace_back(algRes.first, algRes.second, sumResults(algRes.second, true), sumResults(algRes.second, false));
    }
    sort(resVec.begin(), resVec.end(), [](const auto& res1, const auto& res2 ) {
        if(std::get<3>(res1) == std::get<3>(res2)) { // if same #errors sort by #operations
            return std::get<2>(res1) < std::get<2>(res2);
        }
        return std::get<3>(res1) < std::get<3>(res2);
    });
    for(auto& algResult: resVec) {
        file << std::get<0>(algResult) << ","; // algorithm name
        for(auto& travel_result: std::get<1>(algResult)) { // iterating over each travel result
            file << travel_result << ",";
        }
        file << std::get<2>(algResult) << "," << std::get<3>(algResult) << "\n";
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


void Simulation::scanTravelPath(const string &currTravelPath, const string &errorPath) {
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
    for(const auto & entry : fs::directory_iterator(currTravelPath)) {
        if(std::regex_match(entry.path().string(), format) && files.find((entry.path().stem().string().append(".cargo_data"))) == files.end()) {
            file.open(errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << "WARNING: the travel folder " << currTravelPath << " has a cargo_data file that is not in use: " << entry.path().string() << "\n";
            file.close();
        }
    }
}