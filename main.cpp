#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>

#include "so.h"

int main()
{
    SO so;
    std::cout << "Começo dos testes do sistema" <<std::endl;
    std::cout << "----------------------------------" <<std::endl;
    so.teste1();
    std::cout << "----------------------------------" <<std::endl;
    so.teste2();
    std::cout << "----------------------------------" <<std::endl;
    so.teste3();
    std::cout << "----------------------------------" <<std::endl;
    std::cout << "Fim dos testes do sistema" <<std::endl;

    return 0;
}
