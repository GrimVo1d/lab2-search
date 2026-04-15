CXX      ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
SRC_DIR  := src
OBJ_DIR  := build

COMMON_OBJS := $(OBJ_DIR)/teacher.o $(OBJ_DIR)/csv.o
HW2_OBJS    := $(COMMON_OBJS) $(OBJ_DIR)/main.o
GEN_OBJS    := $(COMMON_OBJS) $(OBJ_DIR)/data_gen.o

.PHONY: all clean docs gen-data bench plots

all: hw2 data_gen

hw2: $(HW2_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

data_gen: $(GEN_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

# Размеры от 100 до 1_000_000 как требует задание ЛР2.
gen-data: data_gen
	mkdir -p data
	for n in 100 500 1000 5000 10000 50000 100000 500000 1000000; do \
		./data_gen data/teachers_$$n.csv $$n; \
	done

bench: hw2 gen-data
	mkdir -p results
	./hw2 bench data results/timings.csv results/collisions.csv

plots: bench
	python3 plot.py

docs:
	doxygen Doxyfile

clean:
	rm -rf $(OBJ_DIR) hw2 data_gen results/*.csv results/*.png docs/html
