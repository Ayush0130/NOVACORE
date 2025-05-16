#include "../include/memory.h" 
#include <stdexcept>
#include <cstdint>

Memory::Memory(size_t size) : mem(size, 0) {}

int32_t Memory::load(int addr) const {
    if (addr < 0 || addr >= (int)mem.size())
        throw std::out_of_range("Memory load out of range");
    return mem[addr];
}

void Memory::store(int addr, int32_t value) {
    if (addr < 0 || addr >= (int)mem.size())
        throw std::out_of_range("Memory store out of range");
    mem[addr] = value;
}
