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

string Simulation::getCargoPath(Stowage& stowage, const string &travelDir, const string &port) {
    return travelDir + SUBDIR + port + "_" + std::to_string(stowage._route.getPortNumber()) + ".cargo_data";
}

string Simulation::createPortOutputFile(Stowage& stowage, const string &outputPath, const string &port) {
    string file = outputPath + SUBDIR + port + "_" + std::to_string(stowage._route.getPortNumber()) + ".crane_instructions";
    std::ofstream outfile(file);
    outfile.close();
    return file;
}

string Simulation::createTravelOutputFolder(const string &outputPath, const string &algName, const string &travelName) {
    string path = outputPath + SUBDIR + algName + "_" + travelName + "_crane_instructions";
    fs::create_directory(path);
    return path;
}


void Simulation::writeReaderErrors(int simulationErrors, int algErrors, vector<string> errorMsg, const pair<string, string>& sailInfo, int index) {
    bool errors = false;
    string msg = "";
    for(long unsigned int i = 0; i < errorMsg.size(); i++) {
        if(simulationErrors & pow2(i + index)) {
            if(!errors) {
                if(index == 0) msg.append("Input travel files errors:\n");
                else msg.append("Input cargo data files errors:\n");
            }
            msg.append(errorMsg[i]);
            if(!(algErrors & pow2(i + index))) {
                msg.append("ALGORITHM WARNING: algorithm did not alert this problem\n");
            }
            errors = true;
        }
        else if(algErrors & pow2(i + index)) {
            msg.append("ALGORITHM WARNING: algorithm " + sailInfo.first + " reports a problem the simulator did not find: " + errorMsg[i]);
            errors = true;
        }
    }
    if(errors) {
        _simulationErrors[sailInfo.first][sailInfo.second].append(msg);
    }
}

void Simulation::writeShipErrors(int simulationErrors, int algErrors, const string& travel, const string& algName) {
    if(simulationErrors == 0 && algErrors == 0) {
        return;
    }
    const pair<string, string>& sailInfo = {algName, travel}; // SEPARATOR + "***** ALGORITHM: " + algName + ", TRAVEL: " + travel + " *****\n";
    vector<string> errorMsg;
    errorMsg.emplace_back("\tship plan: a position has an equal number of floors, or more, than the number of floors provided in the first line (ignored)\n");
    errorMsg.emplace_back("\tship plan: a given position exceeds the X/Y ship limits (ignored)\n");
    errorMsg.emplace_back("\tship plan: bad line format after first line or duplicate x,y appearance with same data (ignored)\n");
    errorMsg.emplace_back("\tship plan: travel error - bad first line or file cannot be read altogether (cannot run this travel)\n");
    errorMsg.emplace_back("\tship plan: travel error - duplicate x,y appearance with different data (cannot run this travel)\n");
    errorMsg.emplace_back("\ttravel route: a port appears twice or more consecutively (ignored)\n");
    errorMsg.emplace_back("\ttravel route: bad port symbol format (ignored)\n");
    errorMsg.emplace_back("\ttravel route: travel error - empty file or file cannot be read altogether (cannot run this travel)\n");
    errorMsg.emplace_back("\ttravel route: travel error - file with only a single valid port (cannot run this travel)\n");
    writeReaderErrors(simulationErrors, algErrors, errorMsg, sailInfo);
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


void Simulation::writeCargoErrors(Stowage& stowage, int simulationErrors, int algErrors, vector<unique_ptr<Container>>& containersAtPort, const string& travelName, const string& algName) {
    const pair<string, string> & sailInfo = {algName, travelName};
    string portInfo = "\tcontainers at port " + stowage._route.getCurrentPort() + "_" + std::to_string(stowage._route.getPortNumber()) + ": ";
    vector<string> errorMsg;
    errorMsg.emplace_back(portInfo + "duplicate ID on port (ID rejected)\n");
    errorMsg.emplace_back(portInfo + "ID already on ship (ID rejected)\n");
    errorMsg.emplace_back(portInfo + "bad line format, missing or bad weight (ID rejected)\n");
    errorMsg.emplace_back(portInfo + "bad line format, missing or bad port destination (ID rejected)\n");
    errorMsg.emplace_back(portInfo + "bad line format, ID cannot be read (ignored)\n");
    errorMsg.emplace_back(portInfo + "illegal ID check ISO 6346 (ID rejected)\n");
    errorMsg.emplace_back(portInfo + "file cannot be read altogether (assuming no cargo to be loaded at this port)\n");
    errorMsg.emplace_back(portInfo + "last port has waiting containers (ignored)\n");
    errorMsg.emplace_back(portInfo + "total containers amount exceeds ship capacity (rejecting far containers)\n");
    for(const auto& container: containersAtPort) {
        if(countContainersOnPort(container -> getId(), containersAtPort) > 1) { // 2^10
            if(!(simulationErrors & pow2(10))) { errorMsg[0].append("The duplicated containers: "); }
            simulationErrors |= pow2(10);
            errorMsg[0].append(container -> getId() + " ");
        }
        if(stowage._plan.hasContainer(container -> getId())) { // 2^11
            if(!(simulationErrors & pow2(11))) { errorMsg[1].append("The container: "); }
            simulationErrors |= pow2(11);
            errorMsg[1].append(container -> getId() + " ");
        }
        if(!stowage._route.portInRoute(container -> getDest())) { // 2^13
            if(!(simulationErrors & pow2(13))) { errorMsg[3].append("The containers with port not in route: "); }
            simulationErrors |= pow2(13);
            errorMsg[3].append(container -> getId() + " ");
        }
    }
    if(simulationErrors & pow2(10 )) { errorMsg[0].append("\n"); }
    if(simulationErrors & pow2(11 )) { errorMsg[1].append("\n"); }
    if(simulationErrors & pow2(13 )) { errorMsg[3].append("\n"); }
    if(stowage._route.isLastStop() && !containersAtPort.empty()) {
        simulationErrors |= pow2(17);
    }
    if(stowage._plan.numberOfEmptyCells() + stowage._plan.findContainersToUnload(stowage._route.getCurrentPort()).size() < containersAtPort.size()) { simulationErrors |= pow2(18); }
    writeReaderErrors(simulationErrors, algErrors, errorMsg, sailInfo, 10);
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


void Simulation::scanTravelPath(ShipRoute& route, const string &currTravelPath, const string &errorPath) {
    map<string, int> occurrence;
    std::unordered_set<string> files;
    for(const string& port: route.getRoute()) {
        if(occurrence.count(port) == 0) {
            occurrence[port] = 0;
        }
        occurrence[port]++;
        string cargo = port + "_" + std::to_string(occurrence[port]) + ".cargo_data";
        files.insert(cargo);
        if(!fs::exists(currTravelPath + SUBDIR + cargo)) { // if cargo file doesn't exist, create an empty one
            std::ofstream cargoFile (currTravelPath + SUBDIR + port + "_" + std::to_string(occurrence[port]) + ".cargo_data");
            cargoFile.close();
        }
    }
    std::regex format("(.*)\\.cargo_data");
    std::ofstream file;
    for(const auto & entry : fs::directory_iterator(currTravelPath)) {
        if(std::regex_match(entry.path().string(), format) && files.find((entry.path().stem().string().append(".cargo_data"))) == files.end()) {
            file.open(errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file <<  SEPARATOR << "Simulation warning:\n";
            file << "The travel folder " << currTravelPath << " has a cargo_data file that is not in use: " << entry.path().string() << "\n";
            file.close();
        }
    }
}

void Simulation::setRelevantTravels(vector<string> &travels, const std::unordered_set<string> &invalid) {
    for(const string& travel: invalid) {
        travels.erase(std::remove(travels.begin(), travels.end(), travel), travels.end());
    }
}


bool Simulation::isTravelValid(std::unordered_set<string>& invalidTravels, const string& currTravelPath, const string& travelName, const string& errorPath) {
    pair<int, vector<string >> readRoute = Reader::readShipRoute(getPath(currTravelPath, "route"));
    int readShipPlan = Reader::readShipPlan(getPath(currTravelPath, "ship_plan")).first;
    bool isValidRead = !((readShipPlan & pow2(3)) || (readShipPlan & pow2(4)) || (readRoute.first & pow2(7)) || (readRoute.first & pow2(8)));
    if(!isValidRead) {
        invalidTravels.insert(travelName);
    }
    bool isInInvalid = invalidTravels.find(travelName) != invalidTravels.end();
    ShipRoute route = ShipRoute(readRoute.second);
    scanTravelPath(route, currTravelPath, errorPath);
    return isValidRead && !isInInvalid;
}