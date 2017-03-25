BOOST_VERSION = 1.63.0

PATH_TO_BOOST = /usr/local/Cellar/boost/$(BOOST_VERSION)
PATH_TO_BOOST_INCLUDE = $(PATH_TO_BOOST)/include
PATH_TO_BOOST_LIB = $(PATH_TO_BOOST)/lib

BOOST_LIBS  = -lboost_system
BOOST_LIBS += -lboost_regex

CXX = clang++
CXX_OPTIONS = -std=c++11
INCLUDE = -I$(PATH_TO_BOOST_INCLUDE) -I./include
LDFLAGS = -L$(PATH_TO_BOOST_LIB)
LDLIBS = $(BOOST_LIBS)
RM = rm -rf

SRCDIR = src
CHECKER_SRCS = $(shell echo $(SRCDIR)/{run_checker,checker,triangle}.cpp)
GENERATOR_SRCS = $(shell echo $(SRCDIR)/generate_triangle_info.cpp)

BIN_DIR = bin
CHECKER_TOOL = $(BIN_DIR)/check_triangle_inequality
GENERATOR_TOOL = $(BIN_DIR)/generate_triangle_info

all: bin_dir clean generate_triangle_info checker

checker:
	$(CXX) $(CXX_OPTIONS) $(INCLUDE) $(LDFLAGS) $(LDLIBS) $(CHECKER_SRCS) -o $(CHECKER_TOOL)

generate_triangle_info:
	$(CXX) $(CXX_OPTIONS) $(GENERATOR_SRCS) -o $(GENERATOR_TOOL)
	./$(GENERATOR_TOOL)
	$(RM) $(GENERATOR_TOOL)

bin_dir:
	@mkdir -p $(BIN_DIR)

clean:
	$(RM) $(CHECKER_TOOL) $(GENERATOR_TOOL)
