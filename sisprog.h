#ifndef SISPROG_H
#define SISPROG_H

#include <string>

#include "vm.h"

class SisProg
{
public:
    SisProg();

    void start();
    void login();

    void dir();
    void del(const std::string &file);
    void run(const std::string &file, const std::string &args);
    void end();

private:
    VM vm;
    std::string user = "";
    std::string directory = "";
};

#endif // SISPROG_H
