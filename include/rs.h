#pragma once
#include <vector>
#include <optional>
#include "cdb.h"
#include "rob.h"
#include "rat.h"
#include "cpu.h"
#include "memory.h"
#include "instruction.h"

struct RSEntry {
    bool busy;
    OpType op;
    int Vj, Vk;
    int Qj, Qk;

    float fVj,fVk;
    bool isfloat;
    int destROB;
    int A;
    bool executed;
    int execute_cycles_left; // Countdown for multi-cycle execution
    bool ready_for_broadcast; // Ready to write to CDB but waiting for bus
};

class RS {
public:
    RS(int n, CDB& cdb);
    std::optional<int> findFree();
    void issue(int idx, const Instruction& inst, RAT& rat, CPU& cpu, ROB& rob);
    void execute(CDB& cdb, Memory& mem, ROB& rob);
    void broadcastResult(CDB& cdb, ROB& rob);
    bool allIdle() const {
        for (const auto& e : entries)
            if (e.busy) return false;
        return true;
    }
private:
    std::vector<RSEntry> entries;
    static int getExecutionLatency(OpType op);
};
