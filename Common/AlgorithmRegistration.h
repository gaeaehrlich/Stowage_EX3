#ifndef STOWAGE_ALGORITHMREGISTRAION_H
#define STOWAGE_ALGORITHMREGISTRAION_H

#pragma once

#include <functional>
#include <memory>
#include "../Algorithm/AbstractAlgorithm.h"

class AlgorithmRegistration {
public:
    AlgorithmRegistration(std::function<std::unique_ptr<AbstractAlgorithm>()>);
};

#define REGISTER_ALGORITHM(class_name) \
AlgorithmRegistration register_me_##class_name \
	([]{return std::make_unique<class_name>();} );


#endif //STOWAGE_ALGORITHMREGISTRATION_H