#include "stdafx.h"
#include "vm.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#include "assembler.h"

VM::VM()
{
    for (size_t i = 0; i < memMap.size(); i++)
        memMap[i] = -1;
}

std::stringstream &VM::getOutput()
{
    return output;
}

std::stringstream &VM::getInput()
{
    return input;
}

std::stringstream &VM::getArguments()
{
    return arguments;
}

void VM::saveContext(Process &process)
{
    process.ac = ac;
    process.pc = pc;
    process.switchAddrMode = switchAddrMode;
    process.saveProcess(*this);
}

void VM::restoreContext(const Process &process)
{
    ac = process.ac;
    pc = process.pc;
    switchAddrMode = process.switchAddrMode;
}

void VM::fetchOpcode()
{
	
    BYTE fetched = mem[pc++];
    opcode = fetched >> 4;
    operand = fetched & 0x000F;
}

void VM::fetchOperand()
{
    BYTE fetched = mem[pc++];
    operand = (operand << 8) | fetched;
}

void VM::run()
{
    running = true;
    pc = 0x0001; //set pc to loader
    addrMode = AddrMode::direct;
    bool switchAddrMode = false;
	while (eval() == CycleResult::running);

}

VM::CycleResult VM::eval()
{
    // lidar com modo de enderecamento
    if (switchAddrMode)
    {
        addrMode = AddrMode::indirect;
        switchAddrMode = false;
    }
    else addrMode = AddrMode::direct;

    fetchOpcode();
    switch (opcode)
    {
    case 0x0:   //JP, J: Jump unconditional
        fetchOperand();
        setPc(operand);
        break;
    case 0x1:   //JZ, Z: Jump if zero
        fetchOperand();
        if (ac == 0x0)
            setPc(operand);
        break;
    case 0x2:   //JN, N: Jump if negative
        fetchOperand();
        if ((ac & 0x80) == 0x80)
            setPc(operand);
        break;
    case 0x3:   //CN, C: Control
        switch (operand)
        {
        case 0x0:   //HM: Halt machine
            return CycleResult::finished;
            break;
        case 0x1:   //RI: Return from interrupt
            // Nao implementado nesse projeto
            break;
        case 0x2:   //IN: Indirect
            switchAddrMode = true;
            break;
        default:
            break;
        }
        break;
    case 0x4:   //'+': Add
        fetchOperand();
        ac += getMem(operand);
        break;
    case 0x5:   //'-': Subtract
        fetchOperand();
        ac -= getMem(operand);
        break;
    case 0x6:   //'*': Multiply
        fetchOperand();
        ac *= getMem(operand);
        break;
    case 0x7:   //'/': Divide
        fetchOperand();
        ac /= getMem(operand);
        break;
    case 0x8:   //LD, L: Load from memory
        fetchOperand();
        ac = getMem(operand);
        break;
    case 0x9:   //MM, M: Move to memory
        fetchOperand();
        setMem(operand, ac);
        break;
    case 0xA:   //SC, S: Subrotine call
        fetchOperand();
        callSubrotine(operand);
        break;
    case 0xB:   //OS, O: Operating system call
        //Nao implementado nesse projeto
        break;
    case 0xC:   //IO, I: Input/Output
        switch (operand & 0xC)
        {
        case 0x0:   //GD: Get Data
            switch (operand & 0x3)
            {
            case 0x0:   // Nao usado
                break;
            case 0x1:   // Input de arquivo
                ac = (BYTE) input.get();
                break;
            case 0x2:   // Input de teclado
                if(inputBuffer.empty())
                    std::getline(std::cin, inputBuffer);
                ac = (BYTE) inputBuffer[0];
                inputBuffer.erase(0, 1);
                break;
            case 0x3:   // Argumentos de linha de comando
                ac = (BYTE) arguments.get();
                break;
            default:
                break;
            }
			break;
        case 0x4:   //PD: Put Data
            switch (operand & 0x3)
            {
            case 0x0:   // Nao usado
                break;
            case 0x1:   // Output de arquivo
                output.put((char) ac);
                break;
            case 0x2:   // Output de tela
                std::cout.put((char) ac);
                break;
            case 0x3:   // Nao usado
                break;
            default:
                break;
            }
            break;
        // Interrupts nao utilizados nesse projeto
        //case 0x8: //EI: Enable Interrupt
        //case 0x8: //DI: Disable Interrupt
        default:
            break;
        }
		return CycleResult::IORequest;
    case 0xD:   //WK: work (simular trabalho)
        std::this_thread::sleep_for(std::chrono::milliseconds(operand));
        break;
    // Opcodes nao utilizados
    //case 0xE:
    //case 0xF:
    default:
        break;
    }
	//std::cout << "<PC>" << pc << " \t <OPCode>" << int(opcode) << " \t <AC>" << int(ac) << std::endl;
    return CycleResult::running;
}

void VM::setPc(WORD addr)
{
    switch (addrMode)
    {
    case AddrMode::direct:
    {
        pc = (pc & 0xF000) | (addr & 0x0FFF);
        break;
    }
    case AddrMode::indirect:
    {
        WORD indirectAddr = (pc & 0xF000) | (addr & 0x0FFF);
        pc = (mem[indirectAddr] << 8) | mem[indirectAddr + 1];
        break;
    }
    default:
        break;
    }
}

BYTE VM::getMem(WORD addr)
{
    switch (addrMode)
    {
    case AddrMode::direct:
    {
        WORD directAddr = (pc & 0xF000) | (addr & 0x0FFF);
        return mem[directAddr];
    }
    case AddrMode::indirect:
    {
        WORD indirectAddr = (pc & 0xF000) | (addr & 0x0FFF);
        WORD directAddr = (mem[indirectAddr] << 8) | mem[indirectAddr + 1];
        return mem[directAddr];
    }
    default:
        return 0x0;
    }
}

void VM::setMem(WORD addr, BYTE data)
{
    switch (addrMode)
    {
    case AddrMode::direct:
    {
        WORD directAddr = (pc & 0xF000) | (addr & 0x0FFF);
        mem[directAddr] = data;
        break;
    }
    case AddrMode::indirect:
    {
        WORD indirectAddr = (pc & 0xF000) | (addr & 0x0FFF);
        WORD directAddr = (mem[indirectAddr] << 8) | mem[indirectAddr + 1];
        mem[directAddr] = data;
        break;
    }
    default:
        break;
    }
}

void VM::callSubrotine(WORD addr)
{
    switch (addrMode)
    {
    case AddrMode::direct:
    {
        WORD directAddr = (pc & 0xF000) | (addr & 0x0FFF);
        mem[directAddr++] = (BYTE)(pc >> 8);
        mem[directAddr++] = (BYTE)(pc);
        pc = directAddr;
        break;
    }
    case AddrMode::indirect:
    {
        WORD indirectAddr = (pc & 0xF000) | (addr & 0x0FFF);
        WORD directAddr = (mem[indirectAddr] << 8) | mem[indirectAddr + 1];
        mem[directAddr++] = (BYTE)(pc >> 8);
        mem[directAddr++] = (BYTE)(pc);
        pc = directAddr;
        break;
    }
    default:
        break;
    }
}
