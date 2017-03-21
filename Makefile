boost_version = 1.63.0

all: clean generate_triangle_info main

main:
	clang++ -std=c++11 -I/usr/local/Cellar/boost/$(boost_version)/include -L/usr/local/Cellar/boost/$(boost_version)/lib -lboost_system -lboost_regex main.cpp triangle.cpp -o main

generate_triangle_info:
	clang++ -std=c++11 generate_triangle_info.cpp -o generate_triangle_info
	./generate_triangle_info

clean:
	rm -rf *.o
	rm -rf main triangle generate_triangle_info
