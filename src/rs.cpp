#include "../include/rs.h"
#include <stdexcept>
#include <iostream>
#include <tuple>

RS::RS(int n, CDB& cdb) : entries(n) {
    for (auto& e: entries) e.busy=false;
    cdb.subscribe([this](int tag,int val){
        for (auto& e: entries) if (e.busy) {
            if (e.Qj==tag) { e.Vj=val; e.Qj=-1; }
            if (e.Qk==tag) { e.Vk=val; e.Qk=-1; }
        }
    });
}

std::optional<int> RS::findFree() {
    for (int i=0; i<(int)entries.size(); ++i)
        if (!entries[i].busy) return i;
    return std::nullopt;
}

void RS::issue(int idx, const Instruction& inst, RAT& rat, CPU& cpu, ROB& rob) {
    auto& e = entries[idx];
    e.busy = true;
    e.executed = false;
    e.op = inst.op;
    e.destROB = rob.allocate(inst.op==OpType::ST ? -1 : inst.rd);
    e.A = 0;
    e.Vj = e.Vk = 0;
    e.Qj = e.Qk = -1;
    
    // Initialize new fields:
    e.execute_cycles_left = 0;
    e.ready_for_broadcast = false;

    if (inst.op==OpType::LD||inst.op==OpType::ST)
        e.A = cpu.read(inst.rs1) + inst.imm;

    // src1
    int t1 = rat.get(inst.rs1);
    if (t1>=0) { e.Qj=t1; } else { e.Vj=cpu.read(inst.rs1); e.Qj=-1; }

    // src2
    int reg2 = inst.rs2;  // Store always uses rs2, so no need for conditional
    if (inst.op==OpType::ADD||inst.op==OpType::SUB||inst.op==OpType::MUL
     ||inst.op==OpType::DIV||inst.op==OpType::ST) {
        int t2 = rat.get(reg2);
        if (t2>=0) { e.Qk=t2; } else { e.Vk=cpu.read(reg2); e.Qk=-1; }
    } else { e.Qk=-1; e.Vk=0; }

    if (inst.op!=OpType::ST)
        rat.set(inst.rd, e.destROB);
}

void RS::execute(CDB& cdb, Memory& mem, ROB& rob) {
    for (auto& e: entries) {
        if (e.busy && !e.executed && !e.ready_for_broadcast && e.Qj<0 && e.Qk<0) {
            if (e.execute_cycles_left == 0) {
                // First cycle of execution - initialize
                e.execute_cycles_left = getExecutionLatency(e.op);
                std::cout << "Starting execution of ";
                switch(e.op) {
                    case OpType::ADD: std::cout << "ADD"; break;
                    case OpType::SUB: std::cout << "SUB"; break;
                    case OpType::MUL: std::cout << "MUL"; break;
                    case OpType::DIV: std::cout << "DIV"; break;
                    case OpType::LD:  std::cout << "LOAD"; break;
                    case OpType::ST:  std::cout << "STORE"; break;
                    default: std::cout << "NOP"; break;
                }
                std::cout << ", " << e.execute_cycles_left << " cycles remaining" << std::endl;
            }
            else if (e.execute_cycles_left == 1) {
                // Last cycle of execution - compute result and mark ready for CDB
                int res = 0;
                switch(e.op) {
                    case OpType::ADD: res=e.Vj+e.Vk; break;
                    case OpType::SUB: res=e.Vj-e.Vk; break;
                    case OpType::MUL: res=e.Vj*e.Vk; break;
                    case OpType::DIV: 
                    if(e.Vk==0) {
                        std::cout << "Division by zero error" << std::endl;
                         res=0;
                        throw std::runtime_error("Division by zero");
                       
                    }else res=e.Vj/e.Vk; 
                    break;
                    case OpType::LD:  res=mem.load(e.A); break;
                    case OpType::ST:  mem.store(e.A,e.Vk); break;
                    default: break;
                }
                
                std::cout << "Execution complete, result=" << res << ", ready for CDB" << std::endl;
                e.execute_cycles_left = 0;
                e.ready_for_broadcast = true;
                
                // Store the computed result for later broadcast
                e.Vj = res;  // Reuse Vj to store result
            }
            else {
                // Middle of multi-cycle execution
                e.execute_cycles_left--;
                std::cout << "Executing, " << e.execute_cycles_left 
                          << " cycles remaining" << std::endl;
            }
        }
    }
}

void RS::broadcastResult(CDB& cdb, ROB& rob) {
    // Choose one ready instruction for CDB (priority by ROB order)
    int earliest_rob = -1;
    RSEntry* selected = nullptr;
    
    // Find the oldest instruction ready for broadcasting
    for (auto& e: entries) {
        if (e.busy && e.ready_for_broadcast) {
            if (earliest_rob == -1 || e.destROB < earliest_rob) {
                earliest_rob = e.destROB;
                selected = &e;
            }
        }
    }
    
    // Broadcast the selected result
    if (selected) {
        int result = selected->Vj;  // We stored result in Vj
        
        if (selected->op != OpType::ST) {
            std::cout << "Broadcasting result " << result << " from ROB " << selected->destROB << std::endl;
            rob.markReady(selected->destROB, result);
            cdb.broadcast(selected->destROB, result);
        } else {
            std::cout << "Store complete, marking ROB " << selected->destROB << " as ready" << std::endl;
            rob.markReady(selected->destROB, 0);
        }
        
        // Clear reservation station
        selected->ready_for_broadcast = false;
        selected->executed = true;
        selected->busy = false;
    }
}

int RS::getExecutionLatency(OpType op) {
    switch(op) {
        case OpType::ADD:
        case OpType::SUB: return 1;  
        case OpType::MUL: return 3;  
        case OpType::DIV: return 7;  
        case OpType::LD:  return 2;  
        case OpType::ST:  return 2;  
        default:          return 1;
    }
}
