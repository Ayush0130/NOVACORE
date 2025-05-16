#include "../include/cpu.h"
#include <iostream>

CPU::CPU() { 
    regs.fill(2); 
    regs[0] = 0; // Standard RISC convention: x0 is always 0
}

void CPU::write(int idx, int32_t value) {
    if (idx != 0) regs[idx] = value;
    else std::cout<<"Cannot write to x0\n";
}

int32_t CPU::read(int idx) const {
    return regs[idx];
}

void CPU::dump() const {
    for (int i = 0; i < 32; ++i)
        std::cout << "x" << i << ": " << regs[i] << "\n";
}
