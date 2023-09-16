#include <iostream>
#include "source.hpp"

int main() {
    std::cout << "Calling HelloWorld() from DLL:" << std::endl;
    HelloWorld();
    return 0;
}
