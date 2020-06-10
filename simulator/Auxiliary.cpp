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

void Simulation::writeReaderErrors(int simulationErrors, int algErrors, const pair<string, string>& sailInfo, const string& portInfo) {
    bool errors = false;
    string msg;
    int index = portInfo.empty() ? 0 : 9;
    for(long unsigned int i = 0; i < 9; i++) {
        if(simulationErrors & pow2(i + index)) {
            if(!errors) {
                if(index == 0) msg.append("Input travel files errors:\n");
                else msg.append("Input cargo data files errors at port " + portInfo + ":\n");
            }
            //if(!portInfo.empty()) msg.append(portInfo);
            msg.append(_readerErrors[i + index]);
            if(!(algErrors & pow2(i + index))) {
                msg.append("ALGORITHM WARNING: algorithm did not alert this problem\n");
            }
            errors = true;
        }
        else if(algErrors & pow2(i + index)) {
            string extraError = portInfo.empty() ? _readerErrors[i + index] : portInfo + _readerErrors[i + index];
            msg.append("ALGORITHM WARNING: algorithm " + sailInfo.first + " reports a problem the simulator did not find:\n " + extraError);
            errors = true;
        }
    }
    if(errors) {
        _simulationErrors[sailInfo.first][sailInfo.second].append(msg);
    }
}

void Simulation::writeShipErrors(int algErrors, const string& travel, const string& algName) {
    int simulationErrors = _travels[travel].second;
    if(simulationErrors == 0 && algErrors == 0) {
        return;
    }
    const pair<string, string>& sailInfo = {algName, travel};
    writeReaderErrors(simulationErrors, algErrors, sailInfo);
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
    string portInfo = stowage._route.getCurrentPort() + "_" + std::to_string(stowage._route.getPortNumber());
    for(const auto& container: containersAtPort) {
        if(countContainersOnPort(container -> getId(), containersAtPort) > 1) simulationErrors |= pow2(10); // 2^10
        if(stowage._plan.hasContainer(container -> getId())) simulationErrors |= pow2(11); // 2^11
        if(!stowage._route.portInRoute(container -> getDest())) simulationErrors |= pow2(13); // 2^13
    }
    if(stowage._route.isLastStop() && !containersAtPort.empty()) simulationErrors |= pow2(17);
    if(stowage._plan.numberOfEmptyCells() + stowage._plan.findContainersToUnload(stowage._route.getCurrentPort()).size() < containersAtPort.size()) { simulationErrors |= pow2(18); }
    writeReaderErrors(simulationErrors, algErrors, sailInfo, portInfo);
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

void Simulation::readTravel(const string& currTravelPath, const string& travelName, const string &errorPath) {
    Stowage stowage;
    int readStatus = Reader::readShipRoute(getPath(currTravelPath, "route"), stowage._route) | Reader::readShipPlan(getPath(currTravelPath, "ship_plan"), stowage._plan);
    scanTravelPath(stowage._route, currTravelPath, errorPath);
    _travels[travelName] = {std::move(stowage), readStatus};
}

bool Simulation::isInvalidTravel(const string& travelName) {
    int readStatus = _travels[travelName].second;
    return (readStatus & pow2(3)) || (readStatus & pow2(4)) || (readStatus & pow2(7)) || (readStatus & pow2(8));
}

void Simulation::writeErrors(const string &errorPath) {
    std::ofstream file;
    for(auto const& algorithmErrors: _simulationErrors) {
        for(auto const& travelErrors: algorithmErrors.second) {
            string sailInfo = SEPARATOR + "Algorithm: " + algorithmErrors.first + " on travel: " + travelErrors.first + " had the following errors:\n";
            file.open(errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            file << sailInfo << travelErrors.second;
            file.close();
        }
    }
}

map<string, std::function<unique_ptr<AbstractAlgorithm>()>> Simulation::loadAlgorithmsFromFile(const string &dirPath, const string &errorPath) {
    auto& registrar = AlgorithmRegistrar::getInstance();
    vector<pair<string, string>> algPath; // <algorithm path, algorithm name>
    std::regex format("(.*)\\.so");
    for(const auto & entry : fs::directory_iterator(dirPath)) {
        if(std::regex_match(entry.path().string(), format)) {
            algPath.emplace_back(entry.path().string(), entry.path().stem().string());
        }
    }
    auto errors = registrar.loadAlgorithmFromFile(algPath);
    if(!errors.empty()) writeRegistrarErrors(errorPath, errors);
    return registrar.getAlgorithmFactory();
}

void Simulation::writeRegistrarErrors(const string &path, const vector<pair<string, string>>& errors) {
    std::ofstream file;
    file.open(path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
    for(const auto& error: errors) {
        file << "ERROR: couldn't open algorithm at path: " << error.first << " . The error is: " << error.second << "\n";
    }
    file.close();
}