#pragma once
#include <array>
#include <cstdint>

class CPU {
public:
    std::array<int32_t,32> regs;
    CPU();
    void write(int idx, int32_t value);
    int32_t read(int idx) const;
    void dump() const;
};