cmake_minimum_required(VERSION 3.15)
project(Stowage)

set(CMAKE_CXX_STANDARD 20)
include_directories(../Common)
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
        )

add_library(_208967075_a SHARED
        ./AlgorithmRegistration.h
        ./AbstractAlgorithm.h
        _208967075_a.h
        _208967075_a.cpp
        ${COMMON_SRC}
        )

set_property(TARGET _208967075_a PROPERTY POSITION_INDEPENDENT_CODE ON)