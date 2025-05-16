#include "../include/simulation.h"
#include <iostream>

Simulation::Simulation(const std::vector<Instruction>& prog,
                       int memSize, int robSize, int numRS)
  : program(prog),
    mem(memSize),
    rat(32),
    rob(robSize),
    rs(numRS, cdb)
{}

void Simulation::run() {
    size_t pc = 0;
    int cycles = 0;

    while (true) {
        std::cout << "Cycle " << cycles << ": PC=" << pc;
        
        // 1. Commit (in-order, at most one per cycle)
        rob.commit(cpu, rat, cdb);
        
        // 2. Execute (progress multi-cycle executions)
        rs.execute(cdb, mem, rob);
        
        // 3. CDB Write (arbitrate and broadcast one result per cycle)
        rs.broadcastResult(cdb, rob);

        // 4. Issue (at most one per cycle)
        if (pc < program.size()) {
            auto freeRS = rs.findFree();
            if (freeRS && !rob.isFull()) {
                std::cout << "Issue: " << pc << " to RS[" << *freeRS << "]" << std::endl;
                rs.issue(*freeRS, program[pc], rat, cpu, rob);
                ++pc;
            } else {
                std::cout << "No free RS or ROB full" << std::endl;
            }
        }
        
        ++cycles;
        
        // 5. Exit when all instructions have been issued, RS is empty, and ROB is empty
        if (pc >= program.size() && rs.allIdle() && rob.isEmpty()) {
            break;
        }
        
        // In case i fucked up and infinite loop appears
        if (cycles > 1000) {
            std::cout << "Reached cycle limit, terminating" << std::endl;
            break;
        }
    }

    std::cout << "Finished in " << cycles << " cycles" << std::endl;
    cpu.dump();
}

