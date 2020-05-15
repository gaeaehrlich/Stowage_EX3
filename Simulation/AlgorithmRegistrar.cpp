#include <iostream>
#include "AlgorithmRegistrar.h"

AlgorithmRegistrar AlgorithmRegistrar::_instance;

void AlgorithmRegistrar::registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    _algorithmFactory.push_back(algorithmFactory);
}

AlgorithmRegistrar::~AlgorithmRegistrar() {
    _algorithmFactory.clear();
    _handles.clear();
}

vector<unique_ptr<AbstractAlgorithm>> AlgorithmRegistrar::getAlgorithms() const {
    vector<unique_ptr<AbstractAlgorithm>> algorithms;
    for(auto algorithmFactoryFunc : _algorithmFactory) {
        algorithms.push_back(algorithmFactoryFunc());
    }
    return algorithms;
}

size_t AlgorithmRegistrar::size() const {
    return _algorithmFactory.size();
}

AlgorithmRegistrar &AlgorithmRegistrar::getInstance() {
    return _instance;
}

void AlgorithmRegistrar::loadAlgorithmFromFile(const string &dir_path, const string& error_path) {
    vector<string> alg_path;
    std::regex format("(.*)\\.so");
    for(const auto & entry : fs::directory_iterator(dir_path)) {
        if(std::regex_match(entry.path().string(), format)) {
            alg_path.emplace_back(entry.path().string());
        }
    }
    for(const string& path: alg_path) {
        unique_ptr<void, DlCloser> handle(dlopen(path.data(), RTLD_LAZY));
        if(!handle) {
            std::ofstream file;
            file.open(error_path, std::ios::out | std::ios::app); // file gets created if it doesn't exist and appends to the end
            const char* dlopen_error = dlerror();
            const char* error = dlopen_error ? dlopen_error : "Failed to open shared object.";
            file << "ERROR: couldn't open algorithm at path: " << path << " . The error is: " << error << "\n";
            file.close();
        }
        else {
            _handles.push_back(std::move(handle));
        }
    }
    for(auto& h : _handles) {
        std::cout << "in load from file " << h.get() << std::endl;
    }
}