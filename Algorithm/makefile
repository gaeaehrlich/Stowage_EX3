COMP = g++-8.3.0

SHARED_OBJS = $(NAIVE_SO)
NAIVE_SO = _208967075_a.so
NAIVE = _208967075_a.o

OBJS = Position.o Container.o Operation.o Floor.o ShipPlan.o CraneAux.o Crane.o ShipRoute.o WeightBalanceCalculator.o Reader.o AbstractAlgorithm.o NaiveAlgorithm.o Auxiliary.o Simulation.o

COMMON = ../Common
CPP_COMP_FLAG = -std=c++2a -Wall -Wextra -Werror -pedantic-errors -DNDEBUG -fPIC
CPP_LINK_FLAG = -shared

all: $(SHARED_OBJS)
$(NAIVE_SO): $(OBJS) $(NAIVE)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

$(OBJS) $(NAIVE):
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp -o $@

clean:
	rm -f $(OBJS) $(SHARED_OBJS)
