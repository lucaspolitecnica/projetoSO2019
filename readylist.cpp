#include "stdafx.h"
#include "readylist.h"

ReadyList::ReadyList()
{

}

bool ReadyList::hasProcess()
{
	return processes.size()>0;
}

void ReadyList::addProcess(Process &process, VM &vm)
{
	
	if (processes.size() == 0) {
		vm.restoreContext(process);
	}

	std::list<Process>::iterator it = processes.begin();
	while (it != processes.end()) {
		if (it->prioridade >= process.prioridade) {
			processes.insert(it, process);
			return;
		}
		it++;
	}
	processes.push_back(process);
    
}

VM::CycleResult ReadyList::evalProcess(VM &vm)
{
    if (currentActiveCicles < maxActiveCicles)
    {
        currentActiveCicles++;
    }
    else
    {
        currentActiveCicles = 0;
        if (processes.size() > 1)
        {
            Process proc = processes.front();
            processes.pop_front();
            vm.saveContext(proc);
			std::list<Process>::iterator it = processes.begin();
			while (it != processes.end()) {
				if (it->prioridade > proc.prioridade) {
					processes.insert(it, proc);
					break;
				}
				it++;
			}
			if (it == processes.end())
				processes.push_back(proc);
            vm.restoreContext(processes.front());
        }
    }

    processes.front().loadProcess(vm);
	return vm.eval();
}

Process& ReadyList::waitIOProcess(VM &vm)
{
	Process proc = processes.front();
	processes.pop_front();
	vm.saveContext(proc);
	if (!processes.empty())
		vm.restoreContext(processes.front());
	return proc;
}

Process& ReadyList::removeCurrentProcess(VM &vm)
{
    currentActiveCicles = 0;
	Process p = processes.front();
	p.endProcess(vm);
    processes.pop_front();
	if (!processes.empty())
		vm.restoreContext(processes.front());
	return p;
}

Process &ReadyList::front()
{
    return processes.front();
}
