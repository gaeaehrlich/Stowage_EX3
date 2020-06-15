cmake_minimum_required(VERSION 2.8.9)
project(Stowage)

set(CMAKE_CXX_STANDARD 20)
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
        WeightBalanceCalculator.cpp
        )

add_executable(simulator
        main.cpp
        Crane.h Crane.cpp CraneAux.cpp
        ThreadPool.cpp ThreadPool.h
        Stowage.h
        Simulation.h Simulation.cpp Auxiliary.cpp
        ../algorithm/AbstractAlgorithm.h
        ../algorithm/AlgorithmRegistration.h AlgorithmRegistration.cpp
        AlgorithmRegistrar.h AlgorithmRegistrar.cpp
        ${COMMON_SRC}
        )

target_link_libraries(simulator stdc++fs)
target_link_libraries(simulator ${CMAKE_DL_LIBS})
target_link_libraries(simulator pthread)