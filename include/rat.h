#pragma once
#include <vector>

class RAT {
public:
    RAT(int numArch);
    int get(int arch) const;
    void set(int arch, int phys);
private:
    std::vector<int> table;
};
