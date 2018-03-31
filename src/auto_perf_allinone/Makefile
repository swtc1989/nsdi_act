CXX = g++
CXXFLAGS = -std=c++11 -O2 -w
SRC = ~/ACT/ns-3-dce/build/allinone_main
SRC3 = ~/ACT/ns-3-dce/build/mannual_model1_main
DIR = ~/ACT/ns-3-dce/build/
main: main.cc random_uniform.cpp share.h
	$(CXX) $(CXXFLAGS) -o $(SRC) main.cc -lgsl -lgslcblas
	cp dce.sh $(DIR)

mannual_main: mannual_main.cc random_uniform.cpp share.h
	$(CXX) $(CXXFLAGS) -o $(SRC3) mannual_main.cc -lgsl -lgslcblas

.PHONY: clean
clean:
	rm -f *.o
	rm $(SRC)
