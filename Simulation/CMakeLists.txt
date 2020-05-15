cmake_minimum_required(VERSION 2.8.9)
project(Stowage)

set(CMAKE_CXX_STANDARD 20)
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
        AlgorithmRegistrar.cpp)

add_executable(simulation
        main.cpp
        Crane.h Crane.cpp CraneAux.cpp
        Simulation.h Simulation.cpp Auxillary.cpp
        ../Algorithm/AbstractAlgorithm.h
        ../Algorithm/AlgorithmRegistration.h AlgorithmRegistration.cpp
        AlgorithmRegistrar.h AlgorithmRegistrar.cpp
        ${COMMON_SRC}
        )

target_link_libraries(simulation stdc++fs)
target_link_libraries(simulation ${CMAKE_DL_LIBS})