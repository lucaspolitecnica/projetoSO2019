#ifndef VM_H
#define VM_H

#include <array>
#include <sstream>

#include "types.h"
#include "process.h"

class VM
{
public:
    enum class AddrMode
    {
        direct,
        indirect
    };

    enum class CycleResult
    {
        running,
		IORequest,
        finished
    };

    VM();
    CycleResult eval();
    std::stringstream &getOutput();
    std::stringstream &getInput();
    std::stringstream &getArguments();

    void run();

    void saveContext(Process &process);
    void restoreContext(const Process &process);

//private:
    void fetchOpcode();
    void fetchOperand();
    void setPc(WORD addr);
    BYTE getMem(WORD addr);
    void setMem(WORD addr, BYTE data);
    void callSubrotine(WORD addr);

    BYTE ac = 0;
    WORD pc = 0;
    bool switchAddrMode = false;
    BYTE opcode = 0;
    WORD operand = 0;
    std::array<BYTE, 0x10000> mem;
    std::array<int, 0x10000/blockSize> memMap;
    std::stringstream output;
    std::stringstream input;
    std::stringstream arguments;
    std::string inputBuffer = "";
    bool running = false;
    AddrMode addrMode = AddrMode::direct;
};

#endif // VM_H
