#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <list>
#include <chrono>
#include "types.h"

class VM;

class Process
{
public:
    Process(int id, const std::string &filename, int prioridade);

    void loadProcess(VM &vm);
    void saveProcess(VM &vm);
    void endProcess(VM &vm);

    size_t size;
    WORD initialPc = 0;
    BYTE ac = 0;
    WORD pc = 0;
    bool switchAddrMode = false;
    WORD offset = 0;
    std::vector<Block> blocks;

	//Infos for so
	int id;
	int prioridade;
	std::chrono::duration<double> startTime;
	int executionTime;
	std::list<int> resources;
	int requestedResource;

};

#endif // PROCESS_H
