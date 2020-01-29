#ifndef READYLIST_H
#define READYLIST_H

#include <list>

#include "process.h"
#include "vm.h"


class ReadyList
{
public:
    ReadyList();
	bool hasProcess();
    void addProcess(Process &process, VM &vm);
    VM::CycleResult evalProcess(VM &vm);
	Process& waitIOProcess(VM &vm);
    Process& removeCurrentProcess(VM &vm);
    Process& front();

    constexpr static int maxActiveCicles = 16;

private:
    std::list<Process> processes;
    int currentActiveCicles = 0;
};

#endif // READYLIST_H
