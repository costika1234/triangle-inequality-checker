BOOST_VERSION = 1.63.0

PATH_TO_BOOST = /usr/local/Cellar/boost/$(BOOST_VERSION)
PATH_TO_BOOST_INCLUDE = $(PATH_TO_BOOST)/include
PATH_TO_BOOST_LIB = $(PATH_TO_BOOST)/lib

BOOST_LIBS  = -lboost_system
BOOST_LIBS += -lboost_regex

CXX = clang++
CXX_OPTIONS = -std=c++11
RM = rm -rf

SRCS = run_checker.cpp checker.cpp triangle.cpp
CHECKER_TOOL = check_triangle_inequality
GENERATOR_TOOL = generate_triangle_info

all: clean generate_triangle_info checker

checker:
	$(CXX) $(CXX_OPTIONS) -I$(PATH_TO_BOOST_INCLUDE) \
	                      -L$(PATH_TO_BOOST_LIB) \
						  $(BOOST_LIBS) \
						  $(SRCS) \
						  -o $(CHECKER_TOOL)

generate_triangle_info:
	$(CXX) $(CXX_OPTIONS) generate_triangle_info.cpp \
                          -o $(GENERATOR_TOOL)
	./$(GENERATOR_TOOL)
	$(RM) $(GENERATOR_TOOL)

clean:
	$(RM) *.o *.out
	$(RM) $(CHECKER_TOOL) $(GENERATOR_TOOL)
