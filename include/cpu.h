#pragma once
#include <array>
#include <cstdint>

class CPU {
public:
    std::array<int32_t,32> regs;
    std::array<float,32> fregs;
    CPU();
    void write(int idx, int32_t value);
    int32_t read(int idx) const;

    float fread(int idx) const;
    void fwrite(int idx, float value);
    void dump() const;
};