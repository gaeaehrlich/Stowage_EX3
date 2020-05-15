#ifndef STOWAGE_ALGORITHMREGISTRATION_H
#define STOWAGE_ALGORITHMREGISTRATION_H

#pragma once

#include <functional>
#include <memory>
#include "AbstractAlgorithm.h"
#include "../Simulation/AlgorithmRegistrar.h"


class AlgorithmRegistration {
public:
    AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()>);
};

#define REGISTER_ALGORITHM(class_name) \
AlgorithmRegistration register_me_##class_name \
	([]{return std::make_unique<class_name>();} );

#endif //STOWAGE_ALGORITHMREGISTRATION_H
