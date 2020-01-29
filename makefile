all: sisprog

sisprog: main.cpp vm.cpp assembler.cpp sisprog.cpp
	g++ -O2 -std=c++11 -o sisprog main.cpp vm.cpp assembler.cpp sisprog.cpp

so: main.cpp vm.cpp assembler.cpp process.cpp processlist.cpp so.cpp
	g++ -O2 -std=c++11 -o so main.cpp vm.cpp assembler.cpp process.cpp processlist.cpp so.cpp
