#pragma once
#include "process.h"
#include "vm.h"


class IODeviceList
{
public:
	IODeviceList();
	void IODeviceList::addProcess(const Process &process, VM &vm);
	void IODeviceList::removeProcess(const Process &process);
	Process &IODeviceList::front();
	bool IODeviceList::hasProcess();
	~IODeviceList();
private:
	std::list<Process> processes;
};

