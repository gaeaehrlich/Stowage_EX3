#include <iostream>
#include <utility>
#include "AlgorithmRegistrar.h"


void AlgorithmRegistrar::registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    _algorithmFactory[_currAlgName]= std::move(algorithmFactory);
}

AlgorithmRegistrar::~AlgorithmRegistrar() {
    _algorithmFactory.clear();
    _handles.clear();
}

map<string, std::function<unique_ptr<AbstractAlgorithm>()>> AlgorithmRegistrar::getAlgorithmFactory() const {
    return _algorithmFactory;
}

size_t AlgorithmRegistrar::size() const {
    return _algorithmFactory.size();
}

AlgorithmRegistrar &AlgorithmRegistrar::getInstance() {
    static AlgorithmRegistrar instance;
    return instance;
}

vector<pair<string, string>> AlgorithmRegistrar::loadAlgorithmFromFile(vector<pair<string, string>> algPath) {
    vector<pair<string, string>> errors;
    long unsigned int registered = 0;
    for(auto& path: algPath) {
        _currAlgName = path.second;
        unique_ptr<void, DlCloser> handle(dlopen(path.first.data(), RTLD_LAZY));
        if(!handle) {
            const char* dlopenError = dlerror();
            const char* error = dlopenError ? dlopenError : "Failed to open shared object.";
            errors.push_back({path.first, error});
        }
        else {
            registered++;
            if(registered != _algorithmFactory.size()) {
                errors.push_back({path.first, "The algorithm did not register"});
                registered--;
            }
            else {
                _handles.push_back(std::move(handle));
                _names.push_back(path.second);
            }
        }
    }
    return errors;
}
