SHELL=/bin/bash

UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
  CXX=g++
endif
ifeq ($(UNAME),Darwin)
  CXX=/usr/bin/clang++
endif

CXX_OPTIONS = -std=c++11 -pthread
SRC_INCLUDE = -I./include -I./lib/exprtk
TESTS_INCLUDE = -I./lib/gtestheaders
TESTS_LIB = ./lib/libgtest.a
RM = rm -rf

SRCDIR = src
TESTSDIR = tests

CHECKER_SRCS = $(shell echo $(SRCDIR)/{main,parallel_checker,checker,triangle}.cpp)
GENERATOR_SRCS = $(shell echo $(SRCDIR)/generate_triangle_info.cpp)
TESTS_SRCS = $(shell echo $(TESTSDIR)/*.cpp)

BIN_DIR = bin
CHECKER_TOOL = $(BIN_DIR)/check_triangle_inequality
GENERATOR_TOOL = $(BIN_DIR)/generate_triangle_info
TESTS_BIN = $(BIN_DIR)/tests

all: bin_dir generate_triangle_info checker
tests: bin_dir run_tests

checker:
	$(CXX) $(CXX_OPTIONS) $(SRC_INCLUDE) $(CHECKER_SRCS) -o $(CHECKER_TOOL)

generate_triangle_info:
	$(CXX) $(CXX_OPTIONS) $(SRC_INCLUDE) $(GENERATOR_SRCS) -o $(GENERATOR_TOOL)
	./$(GENERATOR_TOOL)
	$(RM) $(GENERATOR_TOOL)

bin_dir:
	@mkdir -p $(BIN_DIR)

clean:
	$(RM) $(CHECKER_TOOL) $(GENERATOR_TOOL)

run_tests:
	$(CXX) $(CXX_OPTIONS) $(SRC_INCLUDE) $(TESTS_INCLUDE) $(TESTS_SRCS) $(TESTS_LIB) -o $(TESTS_BIN)
	./$(TESTS_BIN)
