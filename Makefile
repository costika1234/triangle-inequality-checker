run: clean generate_header checker

checker:
	clang++ -std=c++11 checker.cpp triangle.cpp -o checker
	time ./checker

generate_header:
	clang++ -std=c++11 -I/usr/local/Cellar/boost/1.61.0/include -L/usr/local/Cellar/boost/1.61.0/lib -lboost_system -lboost_regex generate_header.cpp -o generate_header
	./generate_header

clean:
	rm -rf .o
	rm -rf generate_header
	rm -rf checker
	rm -rf plot

