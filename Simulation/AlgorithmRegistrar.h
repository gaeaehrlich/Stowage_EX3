#ifndef STOWAGE_ALGORITHMREGISTRAR_H
#define STOWAGE_ALGORITHMREGISTRAR_H

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "../Algorithm/AbstractAlgorithm.h"

using std::vector;
using std::string;
using std::unique_ptr;

class AlgorithmRegistrar {
    vector<std::function<unique_ptr<AbstractAlgorithm>()>> algorithmFactories;
    void registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
        algorithmFactories.push_back(algorithmFactory);
    }
public:
    friend struct AlgorithmRegistration;
    vector<unique_ptr<AbstractAlgorithm>> getAlgorithms()const {
        vector<unique_ptr<AbstractAlgorithm>> algorithms;
        for(auto algorithmFactoryFunc : algorithmFactories) {
            algorithms.push_back(algorithmFactoryFunc());
        }
        return algorithms;
    }

    size_t size()const {
        return algorithmFactories.size();
    }

    static AlgorithmRegistrar& getInstance() {
        static AlgorithmRegistrar instance;
        return instance;
    }
};

#endif //STOWAGE_ALGORITHMREGISTRAR_H
