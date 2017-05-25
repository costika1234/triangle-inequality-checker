SHELL=/bin/bash

UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
  CXX=g++
endif
ifeq ($(UNAME),Darwin)
  CXX=/usr/bin/clang++
endif

CXX_OPTIONS = -std=c++11 -pthread
INCLUDE = -I./include -I./lib/exprtk
RM = rm -rf

SRCDIR = src
CHECKER_SRCS = $(shell echo $(SRCDIR)/{main,parallel_checker,checker,triangle}.cpp)
GENERATOR_SRCS = $(shell echo $(SRCDIR)/generate_triangle_info.cpp)

BIN_DIR = bin
CHECKER_TOOL = $(BIN_DIR)/check_triangle_inequality
GENERATOR_TOOL = $(BIN_DIR)/generate_triangle_info

all: bin_dir clean generate_triangle_info checker

checker:
	$(CXX) $(CXX_OPTIONS) $(INCLUDE) $(CHECKER_SRCS) -o $(CHECKER_TOOL)

generate_triangle_info:
	$(CXX) $(CXX_OPTIONS) $(GENERATOR_SRCS) -o $(GENERATOR_TOOL)
	./$(GENERATOR_TOOL)
	$(RM) $(GENERATOR_TOOL)

bin_dir:
	@mkdir -p $(BIN_DIR)

clean:
	$(RM) $(CHECKER_TOOL) $(GENERATOR_TOOL)
