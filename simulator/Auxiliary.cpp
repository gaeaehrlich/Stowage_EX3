#include "Simulation.h"

void Simulation::initPaths(const string& outputPath, string& errorPath, string& resultsPath) {
    errorPath = outputPath + SUBDIR + "simulator.errors";
    std::remove(errorPath.c_str());
    resultsPath = createResultsFile(outputPath);
}

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


void Simulation::writeReaderErrors(const string& errorPath, int simulationErrors, int algErrors, vector<string> errorMsg, const string& sailInfo, int index, bool reportAlg) {
    bool errors = false;
    string msg = sailInfo;
    for(long unsigned int i = 0; i < errorMsg.size(); i++) {
        if(simulationErrors & pow2(i + index)) {
            if(!reportAlg) msg.append("Error " + std::to_string(i + index) + ": " + errorMsg[i]);
            else if(!(algErrors & pow2(i + index))) {
                msg.append("ALGORITHM WARNING: algorithm did not alert error " + std::to_string(i + index) + "\n");
            }
            errors = true;
        }
        else if(reportAlg && algErrors & pow2(i + index)) {
            msg.append("ALGORITHM WARNING: algorithm reports a problem the simulator did not find: error " + std::to_string(i + index) + ": " + errorMsg[i]);
            errors = true;
        }
    }
    if(errors) {
        //std::ofstream file;
        //file.open(errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
        //file << msg;
        //file.close();
    }
}

bool Simulation::writeTravelErrors(const string &errorPath, const string& travel, const string& simOrAlg, int simulationErrors, int algErrors) {
    if(simulationErrors == 0 && algErrors == 0) {
        return true;
    }
    bool simulation = simOrAlg == "Simulation";
    const string& sailInfo = simulation ? SEPARATOR + "***** Simulation reader report *****" :
            SEPARATOR + "***** " + simOrAlg + ": , TRAVEL: " + travel + " *****\n";
    vector<string> errorMsg;
    errorMsg.emplace_back("ship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n");
    errorMsg.emplace_back("ship plan: a given position exceeds the X/Y ship limits (ignored)\n");
    errorMsg.emplace_back("ship plan: bad line format after first line or duplicate x,y appearance with same data (ignored)\n");
    errorMsg.emplace_back("ship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n");
    errorMsg.emplace_back("ship plan: travel error - duplicate x,y appearance with different data (cannot run this travel)\n");
    errorMsg.emplace_back("travel route: a port appears twice or more consecutively (ignored)\n");
    errorMsg.emplace_back("travel route: bad port symbol format (ignored)\n");
    errorMsg.emplace_back("travel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n");
    errorMsg.emplace_back("travel route: travel error - file with only a single valid port (cannot run this travel)\n");
    writeReaderErrors(errorPath, simulationErrors, algErrors, errorMsg, sailInfo, simulation);
    return (simulationErrors & pow2(3)) || (simulationErrors & pow2(4)) || (simulationErrors & pow2(7)) || (simulationErrors & pow2(8));
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
    const string& sailInfo = SEPARATOR + "***** ALGORITHM: " + algName + ", TRAVEL: " + travelName + " *****\n";
    vector<string> errorMsg;
    errorMsg.emplace_back("containers at port: duplicate ID on port (ID rejected)\n");
    errorMsg.emplace_back("containers at port: ID already on ship (ID rejected)\n");
    errorMsg.emplace_back("containers at port: bad line format, missing or bad weight (ID rejected)\n");
    errorMsg.emplace_back("containers at port: bad line format, missing or bad port destination (ID rejected)\n");
    errorMsg.emplace_back("containers at port: bad line format, ID cannot be read (ignored)\n");
    errorMsg.emplace_back("containers at port: illegal ID check ISO 6346 (ID rejected)\n");
    errorMsg.emplace_back("containers at port: file cannot be read altogether (assuming no cargo to be loaded at this port)\n");
    errorMsg.emplace_back("containers at port: last port has waiting containers (ignored)\n");
    errorMsg.emplace_back("containers at port: total containers amount exceeds ship capacity (rejecting far containers)\n");
    for(const auto& container: containersAtPort) {
        if(countContainersOnPort(container -> getId(), containersAtPort) > 1) { // 2^10
            if(!(simulationErrors & pow2(10))) { errorMsg[0].append("The duplicated containers: "); }
            simulationErrors |= pow2(10);
            errorMsg[0].append(container -> getId() + " ");
        }
        if(_plan.hasContainer(container -> getId())) { // 2^11
            if(!(simulationErrors & pow2(11))) { errorMsg[1].append("The container: "); }
            simulationErrors |= pow2(11);
            errorMsg[1].append(container -> getId() + " ");
        }
        if(!_route.portInRoute(container -> getDest())) { // 2^13
            if(!(simulationErrors & pow2(13))) { errorMsg[1].append("The containers with port not in route: "); }
            simulationErrors |= pow2(13);
            errorMsg[3].append(container -> getId() + " ");
        }
    }
    if(simulationErrors & pow2(10 )) { errorMsg[0].append("\n"); }
    if(simulationErrors & pow2(11 )) { errorMsg[1].append("\n"); }
    if(_route.isLastStop() && !containersAtPort.empty()) {
        simulationErrors |= pow2(17);
    }
    if(_plan.numberOfEmptyCells() + _plan.findContainersToUnload(_route.getCurrentPort()).size() < containersAtPort.size()) { simulationErrors |= pow2(18); }
    writeReaderErrors(errorPath, simulationErrors, algErrors, errorMsg, sailInfo, 10);
}


string Simulation::createResultsFile(const string &outputPath) {
    string resultsPath = outputPath + SUBDIR + "simulator.results";
    std::ofstream file(resultsPath);
    file.close();
    return resultsPath;
}

void Simulation::writeResults(const string &path, vector<string> travels) {
    vector<tuple<string, vector<int>, int, int>> results; //alg name, results vector, sum, errors
    std::ofstream file;
    file.open(path, std::ios_base::app);
    file << "RESULTS, ";
    for(auto& algRes: _simulationResults) {
        vector<int> travelsResults;
        for(const string& travel: travels) {
            travelsResults.emplace_back(algRes.second[travel]);
        }
        results.emplace_back(algRes.first, travelsResults, sumResults(travelsResults, true), sumResults(travelsResults, false));
    }
    for(const string& travel: travels) {
        if(travel != "") file << travel << ", ";
    }
    file << "Sum, Num Errors\n";
    sort(results.begin(), results.end(), [](const auto& res1, const auto& res2 ) {
        if(std::get<3>(res1) == std::get<3>(res2)) { // if same #errors sort by #operations
            return std::get<2>(res1) < std::get<2>(res2);
        }
        return std::get<3>(res1) < std::get<3>(res2);
    });
    for(auto& algResult: results) {
        file << std::get<0>(algResult) << ", "; // algorithm name
        for(auto& travel_result: std::get<1>(algResult)) { // iterating over each travel result
            file << travel_result << ", ";
        }
        file << std::get<2>(algResult) << ", " << std::get<3>(algResult) << "\n";
    }
    file.close();
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

void Simulation::setRelevantTravels(vector<string> &travels, const std::unordered_set<string> &invalid) {
    for(const string& travel: invalid) {
        travels.erase(std::remove(travels.begin(), travels.end(), travel), travels.end());
    }
}