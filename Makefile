boost_version = 1.63.0

build: clean main

main:
	clang++ -std=c++11 -I/usr/local/Cellar/boost/$(boost_version)/include -L/usr/local/Cellar/boost/$(boost_version)/lib -lboost_system -lboost_regex main.cpp triangle.cpp -o main

clean:
	rm -rf .o
	rm -rf main
	rm -rf checker
	rm -rf triangle

