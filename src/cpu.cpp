#include "../include/cpu.h"
#include <iostream>

CPU::CPU() { 
    regs.fill(2); 
    fregs.fill(2.5f); // Floating point registers, if needed
    regs[0] = 0;
    fregs[0]=0.0; // Standard RISC convention: x0 is always 0
}

float CPU::fread(int idx) const {
    if (idx < 0 || idx >= 32) {
        std::cerr << "Invalid floating point register index: " << idx << "\n";
        return 0.0f; // Return 0 for invalid index
    }
    return fregs[idx];
}

void CPU::fwrite(int idx, float value) {
    if (idx < 0 || idx >= 32) {
        std::cerr << "Invalid floating point register index: " << idx << "\n";
        return; // Do nothing for invalid index
    }
    if(idx==0){
        std::cerr << "Cannot write to f0 (floating point zero register)\n";
        return; // Do not allow writing to f0
    }
    fregs[idx] = value;
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

    for (int i = 0; i < 32; ++i)
        std::cout << "f" << i << ": " << fregs[i] << "\n";
    std::cout << "CPU state dumped.\n";
}
