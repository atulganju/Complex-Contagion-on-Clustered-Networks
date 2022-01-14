CXXFLAGS = -std=c++2a -pthread -O3 -g
DEPS = node/node.cpp graph/graph.cpp global_defs.cpp
CHOP = -DCHOP
RANDPAIR = -DRAND_PAIR
CLIQUECHOICE = -DCLIQUE_CHOICE
N = 10
M = 4
D = 5

default:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o run_sim $(DEPS) main.cpp

chop:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(CHOP) -o run_sim $(DEPS) main.cpp

rand_pair:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(RANDPAIR) -o run_sim $(DEPS) main.cpp

clique:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(CLIQUECHOICE) -o run_sim $(DEPS) main.cpp

run: default
run_chop: chop
run_rand: rand_pair
run_clique_choice: clique

run run_chop run_rand run_clique_choice:
	./run_sim -n=$(N) -m=$(M) -d=$(D)

clean:
	rm ./run_sim
	rm ../visualizations/*
	rm ./outputs/*