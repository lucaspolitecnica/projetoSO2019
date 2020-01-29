#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>

#include "types.h"

class Assembler
{
public:
    using SymbolMap = std::unordered_map<std::string, WORD>;
    enum class Opcode
    {
        JP,
        JZ,
        JN,
        CN,
        ADD,
        SUB,
        MUL,
        DIV,
        LD,
        MM,
        SC,
        OS,
        IO,
        WK,
        INVALID
    };
    enum class Pseudocode
    {
        ORG,
        K,
        RASC,
        END,
        INVALID
    };

    Assembler();
    Assembler(std::string file);
    void assemble();
    void setAssemblyFile(std::string file);

private:
    void step1();
    void step2();
    Opcode resolveOpcode(const std::string &strOpcode);
    Pseudocode resolvePseudocode(const std::string &strPseudocode);
    int resolveNumber(const std::string &strNumber);

    WORD pc = 0;
    int fileLine;
    BYTE checksum;
    BYTE programSize;
    SymbolMap symbolMap;
    std::ifstream assemblyFile;
    std::ofstream machineCodeFile;
    std::ofstream listing;
};

#endif // ASSEMBLER_H
