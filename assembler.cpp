#include "stdafx.h"
#include "assembler.h"

#include <cctype>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>

// Funcoes helper
namespace
{
bool getNextToken(std::istream& lineStream, std::string& token)
{
    std::string oldToken = token;
    do
    {
        std::getline(lineStream, token, ' ');
        // Conferir se e espaco ASCII (" ", +" " ou -" "), o tokenizador falha nesse caso
        if ((token.length() == 1 && token[0] == '"') || (token.length() == 2 && token[1] == '"'))
        {
            std::string checkSpace = "";
            std::getline(lineStream, checkSpace, ' ');
            if (checkSpace == "\"")
                token.append(" \"");
        }
    } while (token.empty() && !lineStream.eof());
    if (!token.empty() && token != oldToken)
        return true;
    else return false;
}

template <typename T>
std::string numToHex(T num)
{
    std::ostringstream ss;
    ss << std::setfill ('0') << std::setw(sizeof(T)*2) << std::uppercase << std::hex << (unsigned int) num;
    return ss.str();
}

// Prepara linha para ser lida, retorna falso se nao tem conteudo
bool treatLine(std::string &line)
{
    // Conferir se a linha esta vazia
    if (line.empty())
        return false;

    // Apagar comentarios
    {
        auto commentPos = line.find(';');
        if (commentPos != std::string::npos)
            line.erase(commentPos);
    }

    // Conferir se a linha tem conteudo apos apagar comentarios
    if (std::all_of(line.begin(),
                    line.end(),
                    [](char c){
                    return std::isspace(static_cast<unsigned char>(c));
                    }))
        return false;

    // Trocar tabs por espaços
    std::replace(line.begin(), line.end(), '\t', ' ');

    return true;
}
}

Assembler::Assembler()
{
}

Assembler::Assembler(std::string file)
{
    setAssemblyFile(file);
}

void Assembler::assemble()
{
    fileLine = 0;
    programSize = 0;
    step1();
    assemblyFile.clear();
    assemblyFile.seekg(0, std::ios::beg);
    fileLine = 0;
    step2();
    assemblyFile.close();
    listing.close();
    machineCodeFile.close();
}

void Assembler::setAssemblyFile(std::string file)
{
    assemblyFile.open(file);
    if (!assemblyFile.is_open())
        throw std::logic_error("Arquivo " + file + " nao existe ou nao pode ser aberto");

    machineCodeFile.open(file.substr(0, file.length() - 4) + ".txt", std::ios::trunc);
    listing.open(file.substr(0, file.length() - 4) + ".lst", std::ios::trunc);
    if (!machineCodeFile.is_open() || !listing.is_open())
        throw std::logic_error("Nao foi possivel criar arquivos de montagem (progama talvez nao tenha permissao)");
}

void Assembler::step1()
{
    std::string line;

    WORD initialpc = 0;

    while (std::getline(assemblyFile,line))
    {
        fileLine++;

        if (!treatLine(line))
            continue;

        std::istringstream lineStream(line);
        std::string token;

        // Conferir se a linha tem rotulo
        if (!std::isspace(static_cast<unsigned char>(line[0])))
        {
            std::getline(lineStream, token, ' ');
            auto defined = symbolMap.find(token);
            if (defined != symbolMap.end())
                throw std::logic_error("linha " + std::to_string(fileLine) + ": Rotulo " + token + " definido mais de uma vez");
            else symbolMap.insert({token, pc});
        }

        {
            // Procurar a existencia de algum opcode ou pseudocodigo
            bool foundToken = getNextToken(lineStream, token);
            if (!foundToken)
                continue;
        }

        // Conferir se pseudocodigo
        auto pseudoToken = resolvePseudocode(token);
        switch (pseudoToken)
        {
        case Pseudocode::ORG:       // Comeco do programa
        {
            bool foundArg = getNextToken(lineStream, token);
            if (!foundArg)
                throw std::logic_error("linha " + std::to_string(fileLine) + ": Endereco inicial nao especificado");
            pc = initialpc = (WORD) resolveNumber(token);
            break;
        }
        case Pseudocode::K:         // Declaracao de constante
        {
            pc += 1;                // Ocupa um byte
            bool foundArg = getNextToken(lineStream, token);
            if (!foundArg)
                throw std::logic_error("linha " + std::to_string(fileLine) + ": Constante sem valor definido");
            resolveNumber(token); // Conferir se constante valida
            break;
        }
        case Pseudocode::RASC:      // Declaracao de area de rascunho
        {
            bool foundArg = getNextToken(lineStream, token);
            if (!foundArg)
                throw std::logic_error("linha " + std::to_string(fileLine) + ": Area de rascunho sem tamanho definido");
            pc += (WORD) resolveNumber(token);
            break;
        }
        case Pseudocode::END:
        {
            // Nada a fazer nesse passo
            break;
        }
        case Pseudocode::INVALID:   // Nao pseudo, testar se opcode
        {
            auto opcodeToken = resolveOpcode(token);
            switch (opcodeToken)
            {
            // Opcodes de endereco
            case Opcode::JP:
            case Opcode::JZ:
            case Opcode::JN:
            case Opcode::ADD:
            case Opcode::SUB:
            case Opcode::MUL:
            case Opcode::DIV:
            case Opcode::LD:
            case Opcode::MM:
            case Opcode::SC:
            {
                pc += 2; // ocupam dois bytes
                bool foundArg = getNextToken(lineStream, token);
                if (!foundArg)
                    throw std::logic_error("linha " + std::to_string(fileLine) + ": Opcode sem argumento");
                break;
            }
            // Opcode de argumento direto
            case Opcode::CN:
            case Opcode::OS:
            case Opcode::IO:
            case Opcode::WK:
            {
                pc += 1;    //ocupam um byte
                bool foundArg = getNextToken(lineStream, token);
                if (!foundArg)
                    throw std::logic_error("linha " + std::to_string(fileLine) + ": Opcode sem argumento");
                resolveNumber(token);
                break;
            }
            case Opcode::INVALID:
                throw std::logic_error("linha " + std::to_string(fileLine) + ": Opcode invalido");
                break;
            }

        }
        }

    }

    if (pc == initialpc)
        throw std::logic_error("Programa vazio");
    if (((pc - 1) >> 12) != (initialpc >> 12))
        throw std::logic_error("Programa nao cabe em um unico banco de memoria, reconsiderar endereco de carregamento inicial");
    if (pc - initialpc > 0xFF)
        throw std::logic_error("Tamanho do programa excede 255 bytes");
    programSize = (BYTE) (pc - initialpc);
}

void Assembler::step2()
{
    std::string line, unalteredLine;

    // Imprimir endereco inicial e tamanho
    while (std::getline(assemblyFile,line))
    {
        unalteredLine = line;

        if (!treatLine(line))
            continue;

        std::istringstream lineStream(line);
        std::string token;

        // Conferir se o primeiro token e '@'
        getNextToken(lineStream, token);
        if (resolvePseudocode(token) == Pseudocode::ORG)
        {
            getNextToken(lineStream, token);
            pc = (WORD) resolveNumber(token);
            break;
        }
        else throw std::logic_error("Programa nao comeca especificando o endereco inicial");
    }

    listing << numToHex(pc);
    listing << "       ";
    listing << unalteredLine << std::endl;

    machineCodeFile << numToHex(pc) << numToHex(programSize);

    checksum = (BYTE) ((pc >> 8) + (pc & 0x00FF));
    checksum += programSize;

    // Imprimir resto do programa
    while (std::getline(assemblyFile,line))
    {
        unalteredLine = line;

        fileLine++;

        auto oldpc = pc;

        std::string assembledLine = "";

        if (!treatLine(line))
            continue;

        std::istringstream lineStream(line);
        std::string token;

        // Conferir se a linha tem rotulo e ignorar
        if (!std::isspace(static_cast<unsigned char>(line[0])))
            std::getline(lineStream, token, ' ');

        // Guardar se rascunho
        bool PseudoRasc = false;

        // Conferir se só rotulo
        bool foundToken = getNextToken(lineStream, token);
        if (foundToken)
        {
            // Conferir se pseudocodigo
            auto pseudoToken = resolvePseudocode(token);
            switch (pseudoToken)
            {
            case Pseudocode::ORG:       // Comeco do programa - não deve ser encontrado nesse ponto
            {
                throw std::logic_error("Pseudocodigo '@' encontrado mais de uma vez");
            }
            case Pseudocode::K:         // Declaracao de constante
            {
                pc += 1;                // Ocupa um byte
                getNextToken(lineStream, token);
                BYTE constant = (BYTE) resolveNumber(token);
                assembledLine = numToHex(constant);
                checksum += constant;
                break;
            }
            case Pseudocode::RASC:
            {
                getNextToken(lineStream, token);
                auto size = resolveNumber(token);
                assembledLine.append(size*2, '0');  // um byte sao duas letras
                pc += (WORD) size;
                PseudoRasc = true;
                // Rascunho e preenchido por zeros, portanto nao e necessario atualizar checksum
                break;
            }
            case Pseudocode::END:
            {
                bool foundArg = getNextToken(lineStream, token);
                if (foundArg)
                {
                    auto itrSymbol = symbolMap.find(token);
                    if (itrSymbol == symbolMap.end())
                        throw std::logic_error("linha " + std::to_string(fileLine) + ": Rotulo " + token + " nao definido");
                    else
                    {
                        assembledLine = numToHex(itrSymbol->second);
                        checksum += (BYTE) ((itrSymbol->second >> 8) + (itrSymbol->second & 0x00FF));
                    }
                }
                else assembledLine = "0000";    // Programas sem endereco inicial comeca no 0000 que e Halt Machine
                break;
            }
            case Pseudocode::INVALID:   // Nao pseudo, testar se opcode
            {
                auto opcodeToken = resolveOpcode(token);
                switch (opcodeToken)
                {
                // Opcodes de endereco
                case Opcode::JP:
                case Opcode::JZ:
                case Opcode::JN:
                case Opcode::ADD:
                case Opcode::SUB:
                case Opcode::MUL:
                case Opcode::DIV:
                case Opcode::LD:
                case Opcode::MM:
                case Opcode::SC:
                {
                    pc += 2; // ocupam dois bytes
                    getNextToken(lineStream, token);
                    auto itrSymbol = symbolMap.find(token);
                    if (itrSymbol == symbolMap.end())
                        throw std::logic_error("linha " + std::to_string(fileLine) + ": Rotulo " + token + " nao definido");
                    WORD compOp = (WORD) ((static_cast<WORD>(opcodeToken) << 12) | (itrSymbol->second & 0x0FFF));
                    assembledLine = numToHex(compOp);
                    checksum += (BYTE) ((compOp >> 8) + (compOp & 0x00FF));
                    break;
                }
                    // Opcode de argumento direto
                case Opcode::CN:
                case Opcode::OS:
                case Opcode::IO:
                case Opcode::WK:
                {
                    pc += 1;    //ocupam um byte
                    getNextToken(lineStream, token);
                    BYTE compOp = (BYTE) ((static_cast<BYTE>(opcodeToken) << 4) | ((BYTE) (resolveNumber(token) & 0x0F)));
                    assembledLine = numToHex(compOp);
                    checksum += compOp;
                    break;
                }
                case Opcode::INVALID:
                    throw std::logic_error("linha " + std::to_string(fileLine) + ": Opcode invalido");
                    break;
                }

            }
            }
        }

        // Imprimir listagem e codigo montado
        listing << numToHex(oldpc);
        if (PseudoRasc) // tratar rascunnho diferente
        {
            listing << " ....  ";
        }
        else
        {
            listing << ' ';
            listing << assembledLine;
            for (int i = assembledLine.length(); i < 6; i++)
                listing << ' ';
        }
        listing << unalteredLine << std::endl;

        machineCodeFile << assembledLine;
    }

    // Calcular checksum e imprimir-lo
    checksum = -checksum;
    machineCodeFile << numToHex(checksum);
}

Assembler::Opcode Assembler::resolveOpcode(const std::string &strOpcode)
{
    static const std::unordered_map<std::string, Opcode> opcodeMap {
        {"JP", Opcode::JP},
        {"JZ", Opcode::JZ},
        {"JN", Opcode::JN},
        {"CN", Opcode::CN},
        {"+", Opcode::ADD},
        {"-", Opcode::SUB},
        {"*", Opcode::MUL},
        {"/", Opcode::DIV},
        {"LD", Opcode::LD},
        {"MM", Opcode::MM},
        {"SC", Opcode::SC},
        {"OS", Opcode::OS},
        {"IO", Opcode::IO},
        {"WK", Opcode::WK}
    };

    auto itrOpcode = opcodeMap.find(strOpcode);
    if (itrOpcode != opcodeMap.end())
        return itrOpcode->second;
    else return Opcode::INVALID;
}

Assembler::Pseudocode Assembler::resolvePseudocode(const std::string &strPseudocode)
{
    static const std::unordered_map<std::string, Pseudocode> pseudocodeMap {
        {"@", Pseudocode::ORG},
        {"K", Pseudocode::K},
        {"$", Pseudocode::RASC},
        {"#", Pseudocode::END}
    };

    auto itrPseudocode = pseudocodeMap.find(strPseudocode);
    if (itrPseudocode != pseudocodeMap.end())
        return itrPseudocode->second;
    else return Pseudocode::INVALID;
}

int Assembler::resolveNumber(const std::string &strNumber)
{
    size_t pos = 0;
    int sign = 1;
    int num;

    // Resolver sinal
    switch (strNumber[0])
    {
    case '+':
        pos++;
        break;
    case '-':
        sign = -1;
        pos++;
        break;
    default:
        break;
    }

    // Resolver tipo
    switch (strNumber[pos])
    {
    case '/':   // Hexadecimal
    {
        std::istringstream iss(strNumber.substr(pos + 1));
        iss >> std::hex >> num;
        if (iss.fail())
            throw std::logic_error("linha " + std::to_string(fileLine) + ": Esperava numero, encontrado " + strNumber + " ao inves");
        break;
    }
    case '"':   // Caracter ASCII
    {
        if(strNumber.length() != (pos + 3) || strNumber[pos] != '"' || strNumber[pos + 2] != '"')
            throw std::logic_error("linha " + std::to_string(fileLine) + ": Esperava numero, encontrado " + strNumber + " ao inves");
        num = (int) strNumber[pos + 1];
        break;
    }
    default:    // Decimal
    {
        std::istringstream iss(strNumber.substr(pos));
        iss >> num;
        if (iss.fail())
            throw std::logic_error("linha " + std::to_string(fileLine) + ": Esperava numero, encontrado " + strNumber + " ao inves");
        break;
    }
    }

    return sign*num;
}
