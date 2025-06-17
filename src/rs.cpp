#include "../include/rs.h"
#include <stdexcept>
#include <iostream>
#include <tuple>

RS::RS(int n, CDB& cdb) : entries(n) {
    for (auto& e: entries) {
        e.busy=false;
        e.isfloat=false;
    }
    cdb.subscribe([this](int tag,int val,bool isfloat) {
        for (auto& e: entries) if (e.busy) {
             if (e.Qj == tag) { 
                if (!isfloat) {
                    e.Vj = val; 
                } else {
                    e.fVj = *reinterpret_cast<float*>(&val);
                }
                e.Qj = -1; 
            }
            if (e.Qk == tag) { 
                if (!isfloat) {
                    e.Vk = val;
                } else {
                    e.fVk = *reinterpret_cast<float*>(&val);
                }
                e.Qk = -1; 
            }
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
    e.isfloat = (inst.op == OpType::FADD);  // Set flag for FP operation
    
    // Allocate ROB entry with proper floating-point flag
    if (e.isfloat) {
        e.destROB = rob.allocate(inst.rd, true);
    } else {
        e.destROB = rob.allocate(inst.op == OpType::ST ? -1 : inst.rd);
    }
    
    // Initialize values
    e.A = 0;
    e.Vj = e.Vk = 0;
    e.fVj = e.fVk = 0.0f;
    e.Qj = e.Qk = -1;
    e.execute_cycles_left = 0;
    e.ready_for_broadcast = false;
    
    // CRITICAL: Handle dependency checking correctly based on instruction type
    if (e.isfloat) {
        // Floating-point operation dependencies
        int t1 = rat.getF(inst.rs1);
        if (t1 >= 0) {
            e.Qj = t1;
        } else {
            e.fVj = cpu.fread(inst.rs1);
            e.Qj = -1;
        }
        
        int t2 = rat.getF(inst.rs2);
        if (t2 >= 0) {
            e.Qk = t2;
        } else {
            e.fVk = cpu.fread(inst.rs2);
            e.Qk = -1;
        }
        rat.setF(inst.rd, e.destROB);  // Update RAT for floating-point register
    }
    else if (inst.op == OpType::LD || inst.op == OpType::ST) {
        // Memory operation dependencies
        e.A = inst.imm;
        
        int t1 = rat.get(inst.rs1);
        if (t1 >= 0) {
            e.Qj = t1;
        } else {
            e.Vj = cpu.read(inst.rs1);
            e.Qj = -1;
            e.A += e.Vj;
        }
        
        if (inst.op == OpType::ST) {
            int t2 = rat.get(inst.rs2);
            if (t2 >= 0) {
                e.Qk = t2;
            } else {
                e.Vk = cpu.read(inst.rs2);
                e.Qk = -1;
            }
        } else {
            e.Qk = -1;
        }
    }
    else {
        // Integer ALU operation dependencies
        int t1 = rat.get(inst.rs1);
        if (t1 >= 0) {
            e.Qj = t1;
        } else {
            e.Vj = cpu.read(inst.rs1);
            e.Qj = -1;
        }
        
        int t2 = rat.get(inst.rs2);
        if (t2 >= 0) {
            e.Qk = t2;
        } else {
            e.Vk = cpu.read(inst.rs2);
            e.Qk = -1;
        }
    }
    
    // Update RAT
    if (inst.op != OpType::ST) {
        rat.set(inst.rd, e.destROB);
    }
    
    // Debug output
    std::cout << "Issue: ";
    switch(inst.op) {
        case OpType::ADD: std::cout << "ADD"; break;
        case OpType::SUB: std::cout << "SUB"; break;
        case OpType::MUL: std::cout << "MUL"; break;
        case OpType::DIV: std::cout << "DIV"; break;
        case OpType::LD:  std::cout << "LOAD"; break;
        case OpType::ST:  std::cout << "STORE"; break;
        case OpType::FADD: std::cout << "FADD"; break;
        default: std::cout << "NOP"; break;
    }
    std::cout << " to RS[" << idx << "]" << std::endl;
}

void RS::execute(CDB& cdb, Memory& mem, ROB& rob) {
    for (auto& e: entries) {
        if (e.busy && !e.executed && !e.ready_for_broadcast && e.Qj<0 && e.Qk<0) {
            if (e.execute_cycles_left == 0) {
                // First cycle of execution - initialize
                e.execute_cycles_left = getExecutionLatency(e.op); // new execution latency of next process
                std::cout << "Starting execution of ";
                switch(e.op) {
                    case OpType::ADD: std::cout << "ADD"; break;
                    case OpType::SUB: std::cout << "SUB"; break;
                    case OpType::MUL: std::cout << "MUL"; break;
                    case OpType::DIV: std::cout << "DIV"; break;
                    case OpType::LD:  std::cout << "LOAD"; break;
                    case OpType::ST:  std::cout << "STORE"; break;
                    case OpType::FADD: std::cout << "FADD"; break;
                    default: std::cout << "NOP"; break;
                }
                std::cout << ", " << e.execute_cycles_left << " cycles remaining" << std::endl;
            }
            else if (e.execute_cycles_left == 1) {
                // Last cycle of execution - compute result and mark ready for CDB
                int res = 0;

                 if(e.op== OpType::FADD) {
                    float res=e.fVj + e.fVk;
                   std::cout<<"float Execution complete, Vj="<<e.fVj<<", Vk="<<e.fVk<<", res="<<res<<"\n";
                   //std::cout << "float Execution complete, result=" << res << ", ready for CDB" << std::endl;
                e.execute_cycles_left = 0;
                e.ready_for_broadcast = true;
                  e.fVj = res;
                continue;
                 }
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
    
    if (selected) {
        // Handle based on instruction type
        if (selected->isfloat) {
            // This is a float operation
            float fres = selected->fVj;  // Result was stored in fVj
            
            std::cout << "Broadcasting float result " << fres 
                     << " from ROB " << selected->destROB << std::endl;
            
            rob.markReadyFloat(selected->destROB, fres);
            cdb.broadcastFloat(selected->destROB, fres);
        }
        else if (selected->op != OpType::ST) {
            // Integer operation
            int res = selected->Vj;  // Result was stored in Vj
            
            std::cout << "Broadcasting int result " << res 
                     << " from ROB " << selected->destROB << std::endl;
            
            rob.markReady(selected->destROB, res);
            cdb.broadcast(selected->destROB, res);
        }
        else {
            // Store operation
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
        case OpType::FADD: return 4;
        default:          return 1;
    }
}
