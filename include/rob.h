#pragma once
#include <vector>
#include <iostream>
#include <tuple>
#include "cpu.h"
#include "rat.h"
#include "cdb.h"

struct ROBEntry {
    int destArch;

    union {
        int value;
        float fvalue;
    };
    bool isfloat;
    bool ready;
};

class ROB {
public:
    ROB(int capacity);
    bool isFull() const;
    bool isEmpty() const { return count == 0; }
    void markReady(int idx, int value);
    void markReadyFloat(int idx,float value);
    int allocate(int destArch,bool isfloat=false);
    void commit(CPU& cpu, RAT& rat, CDB& cdb);
private:
    int head, tail, count;
    std::vector<ROBEntry> buffer;
};


