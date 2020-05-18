COMP = g++-8.3.0
#put all your object files here
OBJS = Position.o Container.o Operation.o Floor.o ShipPlan.o ShipRoute.o WeightBalanceCalculator.o Reader.o NaiveAlgorithm.o
#The executabel filename DON'T CHANGE

#The shared objects filename
SHARED_OBJS  = _208967075_a.so _208967075_b.so _208967075_c.so

COMMON = ../Common/
ALGORITHM = ./

CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC
CPP_LINK_FLAG = -shared

all: $(SHARED_OBJS)

#a rule for building a simple c++ source file
#use g++ -MM main.cpp to see dependencies
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
WeightBalanceCalculator.o: $(COMMON)WeightBalanceCalculator.cpp $(COMMON)WeightBalanceCalculator.h
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp
Reader.o: $(COMMON)Reader.cpp $(COMMON)Reader.h
	$(COMP) $(CPP_COMP_FLAG) -lstdc++fs -c $(COMMON)$*.cpp
NaiveAlgorithm.o: $(ALGORITHM)NaiveAlgorithm.cpp $(ALGORITHM)NaiveAlgorithm.h $(ALGORITHM)AbstractAlgorithm.h $(ALGORITHM)AlgorithmRegistration.h
	$(COMP) $(CPP_COMP_FLAG) -c $(ALGORITHM)$*.cpp

_208967075_a.o: $(ALGORITHM)_208967075_a.cpp $(ALGORITHM)_208967075_a.h $(ALGORITHM)NaiveAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $(ALGORITHM)$*.cpp
_208967075_b.o: $(ALGORITHM)_208967075_b.cpp $(ALGORITHM)_208967075_b.h $(ALGORITHM)NaiveAlgorithm.h
	$(COMP) $(CPP_COMP_FLAG) -c $(ALGORITHM)$*.cpp
_208967075_c.o: $(ALGORITHM)_208967075_c.cpp $(ALGORITHM)_208967075_c.h $(ALGORITHM)AbstractAlgorithm.h $(ALGORITHM)AlgorithmRegistration.h
	$(COMP) $(CPP_COMP_FLAG) -c $(ALGORITHM)$*.cpp

_208967075_a.so: $(ALGORITHM)_208967075_a.o NaiveAlgorithm.o $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
_208967075_b.so: $(ALGORITHM)_208967075_b.o NaiveAlgorithm.o $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^
_208967075_c.so: $(ALGORITHM)_208967075_c.o $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

clean:
	rm -f $(OBJS) _208967075_a.o _208967075_b.o _208967075_c.o
