#pragma once
#include <vector>
#include "cpu.h"
#include "memory.h"
#include "instruction.h"
#include "rat.h"
#include "rob.h"
#include "rs.h"
#include "cdb.h"

class Simulation {
public:
    Simulation(const std::vector<Instruction>& prog,
               int memSize, int robSize, int numRS);
    void run();
private:
    std::vector<Instruction> program;
    CPU cpu;
    Memory mem;
    RAT rat;
    ROB rob;
    CDB cdb;
    RS rs;
};
