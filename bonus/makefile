COMP = g++-9.3.0

OBJS = Position.o Container.o Operation.o Floor.o ShipPlan.o ShipRoute.o WeightBalanceCalculator.o Reader.o

SHARED_OBJS  = _208967075_c.so

COMMON = ../common/
ALGORITHM = ../algorithm/

CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC -lstdc++fs -fvisibility=hidden -fvisibility-inlines-hidden
CPP_LINK_FLAG = -shared

all: $(SHARED_OBJS)

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
	$(COMP) $(CPP_COMP_FLAG) -c $(COMMON)$*.cpp

_208967075_c.o: ./_208967075_c.cpp ./_208967075_c.h $(ALGORITHM)AbstractAlgorithm.h $(ALGORITHM)AlgorithmRegistration.h
	$(COMP) $(CPP_COMP_FLAG) -c ./$*.cpp

_208967075_c.so: ./_208967075_c.o $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

clean:
	rm -f $(OBJS) _208967075_c.o