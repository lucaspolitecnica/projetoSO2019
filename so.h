#ifndef SO_H
#define SO_H

#include "vm.h"
#include "readylist.h"
#include "memorylist.h"
#include "iodevicelist.h"
#include <array>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime> 
#include <array>
#include <future>
#include <thread>
#include "types.h"

class SO
{
public:
    SO();
	void JobArrival(const std::string &file, int prioridade);
	void stop();

private:
	void run();
	void IOHandling();
	bool blockIO(Process* p);
	void useIO(Process* p);
	void orderList();
	void loadProcessesToReadyList(Process* p);
	void deleteProcessFromReadyList(Process* proc);
	void finishCurrentProcess();
	bool running,ready;
	int idx = 0;
	constexpr static int maxActiveCicles = 16;
	int currentActiveCicles = 0;
    VM vm;
	std::list<int> unavailibleIO;
	std::list<Process> processes;
	std::list<Process*> waitMemlist;
	std::list<Process*> readylist;
	std::list<Process*> waitIODevlist;
	std::list<Process*> waitIOReslist;
	//MemoryList waitMemList;
    //ReadyList readyList;
	//IODeviceList waitIOList;
	//std::array<bool, 6> deviceUsed;
};

#endif // SO_H
