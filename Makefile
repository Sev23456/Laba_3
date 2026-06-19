CXX ?= g++
CPPFLAGS ?= -I. -Iinclude
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic

PROGRAMS := main.exe console_main.exe tests.exe vector_tests.exe matrix_tests.exe
HEADERS := $(wildcard *.h include/*.h include/*.tpp)

.PHONY: all run-tests clean

all: $(PROGRAMS)

main.exe: main.o util.o
	$(CXX) $(CXXFLAGS) $^ -o $@

console_main.exe: console_main.o util.o
	$(CXX) $(CXXFLAGS) $^ -o $@

tests.exe: tests.o
	$(CXX) $(CXXFLAGS) $^ -o $@

vector_tests.exe: vector_tests.o util.o
	$(CXX) $(CXXFLAGS) $^ -o $@

matrix_tests.exe: matrix_tests.o
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

run-tests: tests.exe vector_tests.exe matrix_tests.exe
	./tests.exe
	./vector_tests.exe
	./matrix_tests.exe

clean:
	$(RM) $(PROGRAMS) *.o
