#pragma once
#include <vector>
#include <cstdint>

class Memory {
public:
    std::vector<int32_t> mem;
    Memory(size_t size);
    int32_t load(int addr) const;
    void store(int addr, int32_t value);
};
