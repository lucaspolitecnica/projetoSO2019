#include "stdafx.h"
#include "sisprog.h"

#include <sstream>
#include <iostream>
#include <fstream>    //ifstream, ofstream
#include <algorithm>
#include <cctype>
#include <ctime>
#include <cstdio>

#ifdef _WIN32
#include "ext/dirent.h"
#else
#include <dirent.h>
#endif

#include "assembler.h"

SisProg::SisProg()
{

}

void SisProg::start()
{
    std::cout << "Bem vindo ao sistema de programacao do Renato Rotenberg" << std::endl;

    login();

    std::cout << std::endl;
    std::cout << "Bem vindo " << user << ". O interpretador de comandos esta disponivel." << std::endl;
    std::cout << "Comandos disponiveis: DIR, DEL, RUN, END." << std::endl;
    std::cout << std::endl;

    bool sistemaLigado = true;

    while (sistemaLigado)
    {
        std::cout << user << "@SisProg:~$ ";

        std::string entrada;
        std::getline(std::cin, entrada);

        std::istringstream entradaStream(entrada);
        std::string comando;
        std::getline(entradaStream, comando, ' ');
        std::transform(comando.begin(), comando.end(), comando.begin(), [](unsigned char c){ return std::toupper(c); });

        if (comando == "DIR")
        {
            dir();
        }
        else if (comando == "DEL")
        {
            std::string arquivo;
            std::getline(entradaStream, arquivo, ' ');
            del(arquivo);
        }
        else if (comando == "RUN")
        {
            std::string programa, argumentos;
            std::getline(entradaStream, programa, ' ');
            std::getline(entradaStream, argumentos);
            run(programa, argumentos);
        }
        else if (comando == "END")
        {
            end();
            sistemaLigado = false;
        }
        else std::cout << "Comando " << comando << " invalido" << std::endl;
    }
}

void SisProg::login()
{
    for (;;)
    {
        std::string usuario;
        std::string senha;

        std::cout << "Informe o nome de usuario: ";
        std::getline(std::cin, usuario);
        std::cout << "Informe a senha do usuario: ";
        std::getline(std::cin, senha);

        std::ifstream arquivo("programs/login.txt");
        if (!arquivo.is_open())
            throw std::runtime_error("login.txt nao encontrado");

        while (!arquivo.eof())
        {
            std::string usuarioLido;
            std::string senhaLida;

            std::getline(arquivo, usuarioLido);

            if (usuarioLido.empty() || usuarioLido[0] == '#' )
                continue;

            std::getline(arquivo, senhaLida);

            if (usuario == usuarioLido && senha == senhaLida)
            {
                user = usuario;
                directory = "programs/" + user + "/";
                break;
            }
        }

        if (user != "")
            break;
        else std::cout << "Usuario e/ou senha incorretos, tente novamente." << std::endl;
    }
}

void SisProg::dir()
{
    // Printar assembler manualmente
    std::cout << "assembler" << std::endl;

    DIR *dir;
    struct dirent *ent;

    dir = opendir(directory.substr(0, directory.length()-1).c_str());
    if (dir != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (ent->d_type == DT_REG)
            {
                std::string arquivo = ent->d_name;
                if (arquivo.substr(arquivo.length() - 4) == ".txt")
                    std::cout << arquivo << std::endl;
            }
        }
        closedir (dir);
    }
}

void SisProg::del(const std::string &file)
{
    std::string fullPath = directory + file;
    std::string delFile = fullPath + ".del";
    if (std::rename(fullPath.c_str(), delFile.c_str()))
        std::cout << "Falha ao deletar " << file << std::endl;
    else std::cout << "Arquivo " << file << " renomeado para " << file << ".del" << std::endl;
}

void SisProg::run(const std::string &file, const std::string &args)
{
    // Conferir se o programa requisitado foi o assembler
    if (file == "assembler")
    {
        try
        {
            std::string fullPath = directory + args;
            Assembler ass(fullPath);
            ass.assemble();
            std::cout << "Montagem de " << args << " concluida" << std::endl;
        }
        catch (std::exception& e)
        {
            std::cout << "Erro na montagem: " << e.what() << '.' << std::endl;
            std::cout << "Montagem abortada." << std::endl;
        }
        return;
    }

    vm.getInput().clear();
    vm.getInput().str("");
    vm.getArguments().clear();
    vm.getArguments().str("");
    vm.getOutput().clear();
    vm.getOutput().str("");

    std::ifstream exeFile(directory + file);
    if (!exeFile.is_open())
    {
        std::cout << "Programa " << file << " nao encontrado." << std::endl;
        return;
    }

    std::string exe = "";
    exeFile >> exe;
    vm.getInput() << exe;
    vm.getArguments() << args;

    vm.run();

    if (!vm.getOutput().str().empty())
    {
        std::time_t tt;
        std::time(&tt);
		tm local_tm;
        localtime_s(&local_tm,&tt);
        std::string outputName =
                directory +
                std::to_string(local_tm.tm_year + 1900) + '-' +
                std::to_string(local_tm.tm_mon + 1) + '-' +
                std::to_string(local_tm.tm_mday) + '-' +
                std::to_string(local_tm.tm_hour) + ':' +
                std::to_string(local_tm.tm_min) + ':' +
                std::to_string(local_tm.tm_sec) + ".txt";
        std::ofstream outputFile(outputName, std::ios::trunc);
        outputFile << vm.getOutput().str();
    }

    std::cout << std::endl;
}

void SisProg::end()
{
    // Apagar arquivos com a extensão .del
    DIR *dir;
    struct dirent *ent;

    dir = opendir(directory.substr(0, directory.length()-1).c_str());
    if (dir != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            if (ent->d_type == DT_REG)
            {
                std::string arquivo = ent->d_name;
                if (arquivo.substr(arquivo.length() - 4) == ".del")
                {
                    std::string fullPath = directory + arquivo;
                    if (!std::remove(fullPath.c_str()))
                        std::cout << "Arquivo " << arquivo << " apagado." << std::endl;
                }
            }
        }
        closedir (dir);
    }

    std::cout << std::endl;
    std::cout << "Sistema encerrado com sucesso, tchau!" << std::endl;
}
