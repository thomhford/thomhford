# Makefile - builds program

# Macros:
O = -o
C = -c
CXX = c++
CFLAGS = -std=c++17
CXXSOURCE = main.cpp cmdline.cpp Expr.cpp parse.cpp test.cpp test_suppl.cpp Val.cpp
HEADERS = cmdline.h Expr.h catch.h parse.h Val.h
OBJS = $(CXXSOURCE:.cpp=.o)

# To build executable:
# make all
all: msdscript

# Can also build using:
# make msdscript
# Creates necessary files implicitly and links files into executable

test_msdscript:
	c++ exec.cpp test_msdscript.cpp -std=c++11 -o $@

msdscript : $(OBJS)
	$(CXX) $(CFLAGS) $(O) $@ $(OBJS)

Expr.o : cmdline.cpp $(HEADERS)
	$(CXX) $(CFLAGS) $(C) Expr.cpp

cmdline.o : cmdline.cpp $(HEADERS)
	$(CXX) $(CFLAGS) $(C) cmdline.cpp

main.o : main.cpp $(HEADERS)
	$(CXX) $(CFLAGS) $(C) main.cpp

parse.o : parse.cpp $(HEADERS)
	$(CXX) $(CFLAGS) $(C) parse.cpp

test.o : test.cpp $(HEADERS)
	$(CXX) $(CFLAGS) $(C) test.cpp

test_suppl.o : test_suppl.cpp $(HEADERS)
	$(CXX) $(CFLAGS) $(C) test_suppl.cpp

# Used to remove unnecessary files
.PHONY: clean
clean:
	rm -f *.o *.out msdscript
.PHONY: cleantest
cleantest:
	rm -f *.o *.out test_msdscript

# Used to run tests
.PHONY: test
test: msdscript
	./msdscript --test

# Documentation
.PHONY: doc
doc:
	cd Documentation && doxygen

# Open Documentation for reading
.PHONY: showdoc
showdoc:
	cd Documentation/html && open index.html
# Used to run executable
run: msdscript
	./msdscript

run_test: cleantest test_msdscript
	./test_msdscript