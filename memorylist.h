#ifndef MEMORYLIST_H
#define MEMORYLIST_H

#include <list>

#include "process.h"
#include "vm.h"


class MemoryList
{
public:
	class MemoryList();
	void addProcess(const Process &process, VM &vm);
	void removeProcess(const Process &process);
	Process& front();
	bool hasProcess();

private:
	std::list<Process> processes;
};

#endif // WAITINGLIST_H
