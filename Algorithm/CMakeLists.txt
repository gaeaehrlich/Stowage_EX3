cmake_minimum_required(VERSION 2.8.9)
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
        AlgorithmRegistration.h
        AbstractAlgorithm.h
        NaiveAlgorithm.h NaiveAlgorithm.cpp
        _208967075_a.h _208967075_a.cpp
        ${COMMON_SRC}
        )
add_library(_208967075_b SHARED
        AlgorithmRegistration.h
        AbstractAlgorithm.h
        NaiveAlgorithm.h NaiveAlgorithm.cpp
        _208967075_b.h _208967075_b.cpp
        ${COMMON_SRC}
        )
set_property(TARGET _208967075_a PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET _208967075_b PROPERTY POSITION_INDEPENDENT_CODE ON)