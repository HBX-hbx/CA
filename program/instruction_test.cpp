#include "instructions.h"

#include <iostream>

int main() {
    unsigned x;
    std::cout << "Instruction: 0x";
    std::cin >> std::hex >> x;
    Instruction inst(x);
    std::cout << inst << std::endl;
    return 0;
}