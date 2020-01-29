#include "stdafx.h"
#include "memorylist.h"

MemoryList::MemoryList()
{

}

void MemoryList::addProcess(const Process &process, VM &vm)
{
	processes.push_back(process);

}
void MemoryList::removeProcess(const Process &process)
{
	processes.pop_front();

}
Process &MemoryList::front()
{
	return processes.front();
}

bool MemoryList::hasProcess()
{
	return processes.size()>0;
}
