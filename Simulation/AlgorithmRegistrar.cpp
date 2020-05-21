#include <iostream>
#include "AlgorithmRegistrar.h"


void AlgorithmRegistrar::registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    _algorithmFactory.push_back(algorithmFactory);
}

AlgorithmRegistrar::~AlgorithmRegistrar() {
    _algorithmFactory.clear();
    for(auto &handle : _handles){
        ;
    }
    _handles.clear();
}

vector<pair<string, unique_ptr<AbstractAlgorithm>>> AlgorithmRegistrar::getAlgorithms() const {
    vector<pair<string, unique_ptr<AbstractAlgorithm>>> algorithms;
    for(long unsigned int i = 0; i < _algorithmFactory.size(); i++) {
        algorithms.push_back({_names[i], _algorithmFactory[i]()});
    }
    return algorithms;
}

size_t AlgorithmRegistrar::size() const {
    return _algorithmFactory.size();
}

AlgorithmRegistrar &AlgorithmRegistrar::getInstance() {
    static AlgorithmRegistrar instance;
    return instance;
}

void AlgorithmRegistrar::loadAlgorithmFromFile(const string &dirPath, const string& errorPath) {
    vector<pair<string, string>> algPath; // <algorithm path, algorithm name>
    std::regex format("(.*)\\.so");
    for(const auto & entry : fs::directory_iterator(dirPath)) {
        if(std::regex_match(entry.path().string(), format)) {
            algPath.emplace_back(entry.path().string(), entry.path().stem().string());
        }
    }
    long unsigned int registered = 0;
    for(auto& path: algPath) {
        unique_ptr<void, DlCloser> handle(dlopen(path.first.data(), RTLD_LAZY));
        if(!handle) {
            std::ofstream file;
            file.open(errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            const char* dlopenError = dlerror();
            const char* error = dlopenError ? dlopenError : "Failed to open shared object.";
            file << "ERROR: couldn't open algorithm at path: " << path.first << " . The error is: " << error << "\n";
            file.close();
        }
        else {
            registered++;
            if(registered != _algorithmFactory.size()) {
                std::ofstream file;
                file.open(errorPath, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
                file << "ERROR: couldn't open algorithm at path: " << path.first << " . The algorithm did not register\n";
                file.close();
                registered--;
            }
            else {
                _handles.push_back(std::move(handle));
                _names.push_back(path.second);
            }
        }
    }
}