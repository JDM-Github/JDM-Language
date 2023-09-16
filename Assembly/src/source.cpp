#include <iostream>
#include "source.hpp"
extern "C"


__declspec(dllexport)
HelloWorld::HelloWorld() {
    std::cout << "Hello, World from DLL!" << std::endl;
}

