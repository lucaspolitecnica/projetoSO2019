#include "stdafx.h"
#include "process.h"

#include<fstream>
#include <iostream>

#include "vm.h"

Process::Process(int id, const std::string &exeStr, int prioridade) :
    id(id)
{
    std::vector<BYTE> exe;
    for (size_t i = 0; i < exeStr.size()/2; i++)
        exe.push_back(ASCII2HEX(exeStr[2*i], exeStr[2*i + 1]));

    initialPc = (exe[0] << 8 | exe[1]);

    size = exe[2];

    blocks.resize(size/blockSize + 1);
    for (size_t i = 3; i < exe.size() - 3; i++)
        blocks[(i-3)/blockSize][(i-3)%blockSize] = exe[i];

    pc = (exe[exe.size() - 3] << 8 | exe[exe.size() - 2]);

    BYTE checksum = 0;
    for (size_t i = 0; i < exe.size(); i++)
        checksum += exe[i];

    if (checksum != 0)
        throw std::exception();
	this->prioridade=prioridade;
}

void Process::loadProcess(VM &vm)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        if (vm.memMap[i + initialPc/blockSize] != id)
        {
			//std::cout << id << "<Loaded process to RAM>" << initialPc + i*blockSize << std::endl;
            for (size_t j = 0; j < blockSize; j++)
                vm.mem[initialPc + i*blockSize + j] = blocks[i][j];
            vm.memMap[i + initialPc/blockSize] = id;
        }
    }
}

void Process::saveProcess(VM &vm)
{
    for (size_t i = 0; i < blocks.size(); i++)
    {
        for (size_t j = 0; j < blockSize; j++)
            blocks[i][j] = vm.mem[initialPc + i*blockSize + j];
    }
}

void Process::endProcess(VM &vm)
{
    for (size_t i = 0; i <= size/blockSize; i++)
        vm.memMap[i + initialPc/blockSize] = -1;
}
