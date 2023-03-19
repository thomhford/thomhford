# Makefile - builds program
# To build executable:
# make all


all: run

MallocReplacement:
	c++ allocator.cpp HashTable.cpp allocator_testing.cpp main.cpp -std=c++17 -o MallocReplacement

# Used to remove unnecessary files
.PHONY: clean
clean:
	rm -f *.o *.out MallocReplacement

# Used to run executable
.PHONY: run
run: MallocReplacement
	./MallocReplacement --test