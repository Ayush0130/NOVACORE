#pragma once
#include <vector>
#include <iostream>
#include <tuple>
#include "cpu.h"
#include "rat.h"
#include "cdb.h"

struct ROBEntry {
    int destArch;
    int value;
    bool ready;
};

class ROB {
public:
    ROB(int capacity);
    bool isFull() const;
    bool isEmpty() const { return count == 0; }
    int allocate(int destArch);
    void markReady(int idx, int value);
    void commit(CPU& cpu, RAT& rat, CDB& cdb);
private:
    int head, tail, count;
    std::vector<ROBEntry> buffer;
};


