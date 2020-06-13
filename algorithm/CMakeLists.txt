cmake_minimum_required(VERSION 2.8.9)
project(Stowage)

set(CMAKE_CXX_STANDARD 20)
include_directories(../common)
set(COMMON_SRC
        ../common/Container.h
        ../common/Container.cpp
        ../common/Floor.h
        ../common/Floor.cpp
        ../common/Operation.h
        ../common/Operation.cpp
        ../common/ShipPlan.h
        ../common/ShipPlan.cpp
        ../common/ShipRoute.h
        ../common/ShipRoute.cpp
        ../common/Reader.h
        ../common/Reader.cpp
        ../common/Position.h
        ../common/Position.cpp
        ../common/WeightBalanceCalculator.h
        ../common/WeightBalanceCalculator.cpp
        )

add_library(_208967075_a SHARED
        ./AlgorithmRegistration.h
        ./AbstractAlgorithm.h
        BaseAlgorithm.h BaseAlgorithm.cpp
        _208967075_a.h
        _208967075_a.cpp
        ${COMMON_SRC}
        )

add_library(_208967075_b SHARED
        ./AlgorithmRegistration.h
        ./AbstractAlgorithm.h
        BaseAlgorithm.h BaseAlgorithm.cpp
        _208967075_b.h
        _208967075_b.cpp
        ${COMMON_SRC}
        )

set_property(TARGET _208967075_a PROPERTY POSITION_INDEPENDENT_CODE ON)