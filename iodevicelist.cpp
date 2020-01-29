#include "stdafx.h"
#include "iodevicelist.h"


IODeviceList::IODeviceList()
{
}


void IODeviceList::addProcess(const Process &process, VM &vm)
{
	processes.push_back(process);

}
void IODeviceList::removeProcess(const Process &process)
{
	processes.pop_front();

}
Process &IODeviceList::front()
{
	return processes.front();
}

bool IODeviceList::hasProcess()
{
	return processes.size()>0;
}

IODeviceList::~IODeviceList()
{
}
