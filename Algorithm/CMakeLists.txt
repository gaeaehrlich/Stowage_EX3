cmake_minimum_required(VERSION 3.15)
project(Stowage)

set(CMAKE_CXX_STANDARD 17)
set(COMMON_SRC
        ../Common/Container.h
        ../Common/Container.cpp
        ../Common/Floor.h
        ../Common/Floor.cpp
        ../Common/Operation.h
        ../Common/Operation.cpp
        ../Common/ShipPlan.h
        ../Common/ShipPlan.cpp
        ../Common/ShipRoute.h
        ../Common/ShipRoute.cpp
        ../Common/Reader.h
        ../Common/Reader.cpp
        ../Common/Position.h
        ../Common/Position.cpp
        ../Common/WeightBalanceCalculator.h
        ../Common/WeightBalanceCalculator.cpp
        ../Common/AlgorithmRegistration.h
        )

add_library(algorithm SHARED
        ./AlgorithmRegistration.h
        ./AbstractAlgorithm.h
        ./NaiveAlgorithm.h
        ./NaiveAlgorithm.cpp
        ${COMMON_SRC}
        )