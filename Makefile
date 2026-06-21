CXX ?= g++
CPPFLAGS ?= -I. -Iinclude
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic

PROGRAMS := main.exe console_main.exe tests.exe vector_tests.exe matrix_tests.exe
HEADERS := $(wildcard *.h include/*.h include/*.tpp)

.PHONY: all run-tests clean

all: $(PROGRAMS)

main.exe: main.cpp util.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) main.cpp util.cpp -o $@

console_main.exe: console_main.cpp util.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) console_main.cpp util.cpp -o $@

tests.exe: tests.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) tests.cpp -o $@

vector_tests.exe: vector_tests.cpp util.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) vector_tests.cpp util.cpp -o $@

matrix_tests.exe: matrix_tests.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) matrix_tests.cpp -o $@

run-tests: tests.exe vector_tests.exe matrix_tests.exe
	./tests.exe
	./vector_tests.exe
	./matrix_tests.exe

clean:
	$(RM) $(PROGRAMS) *.o
