#include "../Algorithm/AlgorithmRegistration.h"
#include "AlgorithmRegistrar.h"

AlgorithmRegistration::AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
}