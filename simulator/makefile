COMP = g++-9.3.0

OBJS = Position.o Container.o Operation.o Floor.o ShipPlan.o ShipRoute.o Reader.o WeightBalanceCalculator.o CraneAux.o Crane.o AlgorithmRegistrar.o AlgorithmRegistration.o Auxiliary.o ThreadPool.o Simulation.o main.o
EXEC = simulator
COMMON = ../common/
SIMULATOR= ./
ALGORITHM = ../algorithm/

CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG
CPP_LINK_FLAG = -ldl -export-dynamic -lstdc++fs -pthread

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@
Position.o: $(COMMON)Position.cpp $(COMMON)Position.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
Container.o: $(COMMON)Container.cpp $(COMMON)Container.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
Operation.o: $(COMMON)Operation.cpp $(COMMON)Operation.h $(COMMON)Container.h $(COMMON)Position.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
Floor.o: $(COMMON)Floor.cpp $(COMMON)Floor.h $(COMMON)Container.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
ShipPlan.o: $(COMMON)ShipPlan.cpp $(COMMON)ShipPlan.h $(COMMON)Floor.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
ShipRoute.o: $(COMMON)ShipRoute.cpp $(COMMON)ShipRoute.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
Reader.o: $(COMMON)Reader.cpp $(COMMON)Reader.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
WeightBalanceCalculator.o: $(SIMULATOR)WeightBalanceCalculator.cpp $(COMMON)WeightBalanceCalculator.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
CraneAux.o: $(SIMULATOR)CraneAux.cpp $(SIMULATOR)Crane.h $(COMMON)Operation.h $(COMMON)Reader.h $(COMMON)ShipPlan.h $(COMMON)WeightBalanceCalculator.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
Crane.o: $(SIMULATOR)Crane.cpp $(SIMULATOR)Crane.h $(COMMON)Operation.h $(COMMON)Reader.h $(COMMON)ShipPlan.h $(COMMON)WeightBalanceCalculator.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
AlgorithmRegistrar.o: $(SIMULATOR)AlgorithmRegistrar.cpp $(SIMULATOR)AlgorithmRegistrar.h $(ALGORITHM)AbstractAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
AlgorithmRegistration.o: $(SIMULATOR)AlgorithmRegistration.cpp $(ALGORITHM)AlgorithmRegistration.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
Auxiliary.o: $(SIMULATOR)Auxiliary.cpp $(SIMULATOR)Simulation.h $(SIMULATOR)AlgorithmRegistrar.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
ThreadPool.o: $(SIMULATOR)ThreadPool.cpp $(SIMULATOR)ThreadPool.h $(ALGORITHM)AbstractAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
Simulation.o: $(SIMULATOR)Simulation.cpp $(SIMULATOR)Simulation.h $(SIMULATOR)AlgorithmRegistrar.h $(SIMULATOR)Stowage.h $(SIMULATOR)ThreadPool.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
main.o: main.cpp $(SIMULATOR)Simulation.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)
