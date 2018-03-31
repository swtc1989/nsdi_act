CXX = g++
CXXFLAGS = -std=c++11 -g
SRC = offline_checker

offline_checker: offline_checker.cc  share.h
	$(CXX) $(CXXFLAGS) -o $(SRC) offline_checker.cc

.PHONY: clean
clean:
	rm -f *.o
	rm $(SRC)
