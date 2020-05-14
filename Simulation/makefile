COMP = g++-8.3.0

OBJS = Position.o Container.o Operation.o Floor.o ShipPlan.o CraneAux.o Crane.o ShipRoute.o WeightBalanceCalculator.o Stowage.o Reader.o Algorithm.o NaiveAlgorithm.o Auxiliary.o Simulation.o main.o
EXEC = simulation
COMMON = ../Common/
ALGORITHM = ../Algorithm/
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG
CPP_LINK_FLAG = -lstdc++fs

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@
Position.o: $(COMMON)Position.cpp $(COMMON)Position.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
Container.o: $(COMMON)Container.cpp $(COMMON)Container.h $(COMMON)Position.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
Operation.o: $(COMMON)Operation.cpp $(COMMON)Operation.h $(COMMON)Container.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
Floor.o: $(COMMON)Floor.cpp $(COMMON)Floor.h $(COMMON)Container.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
ShipPlan.o: $(COMMON)ShipPlan.cpp $(COMMON)ShipPlan.h $(COMMON)Floor.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
CraneAux.o:
Crane.o: $(COMMON)Crane.cpp $(COMMON)CraneAux.h $(COMMON)Operation.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
ShipRoute.o: $(COMMON)ShipRoute.cpp $(COMMON)ShipRoute.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
WeightBalanceCalculator.o: $(ALGORITHM)WeightBalanceCalculator.cpp $(ALGORITHM)WeightBalanceCalculator.h $(COMMON)ShipPlan.h
	$(COMP) $(CPP_COMP_FLAG) -c $(ALGORITHM)$*.cpp
Reader.o: $(ALGORITHM)Reader.cpp $(ALGORITHM)Reader.h
	$(COMP) $(CPP_COMP_FLAG) -c $(ALGORITHM)$*.cpp
Algorithm.o: $(ALGORITHM)Algorithm.cpp $(ALGORITHM)Algorithm.h $(ALGORITHM)WeightBalanceCalculator.h $(ALGORITHM)Reader.h $(COMMON)ShipPlan.h $(COMMON)ShipRoute.h $(ALGORITHM)WeightBalanceCalculator.h
	$(COMP) $(CPP_COMP_FLAG) -c $(ALGORITHM)$*.cpp
NaiveAlgorithm.o: $(ALGORITHM)NaiveAlgorithm.cpp $(ALGORITHM)NaiveAlgorithm.h $(ALGORITHM)Algorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $(ALGORITHM)$*.cpp
Auxiliary.o: $(SIMULATOR)Simulation.cpp $(SIMULATOR)Simulation.h $(ALGORITHM)NaiveAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
Simulation.o: $(SIMULATOR)Simulation.cpp $(SIMULATOR)Simulation.h $(ALGORITHM)NaiveAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $(SIMULATOR)$*.cpp
main.o: main.cpp $(SIMULATOR)Simulation.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)
