#include "../include/rob.h"
#include <stdexcept>
#include <iostream>
#include "../include/rat.h"
#include "../include/cdb.h"

ROB::ROB(int capacity)
 : head(0), tail(0), count(0), buffer(capacity) {}

bool ROB::isFull() const { return count == (int)buffer.size(); }

int ROB::allocate(int destArch) {
    if (isFull()) {
        throw std::runtime_error("ROB full");
        // std::cout << "ROB full" << std::endl;
        // return -1;
    }
    int idx = tail;
    buffer[tail] = {destArch, 0, false};
    tail = (tail+1) % buffer.size();
    ++count;
    return idx;
}

void ROB::markReady(int idx, int value) {
    buffer[idx].value = value;
    buffer[idx].ready = true;
}

void ROB::commit(CPU& cpu, RAT& rat, CDB& cdb) {
    if (count == 0) {
        std::cout << "ROB empty, nothing to commit" << std::endl;
        return;
    }
    
    auto& entry = buffer[head];
    std::cout << "ROB head=" << head << " ready=" << entry.ready 
              << " destArch=" << entry.destArch 
              << " value=" << entry.value << std::endl;
              
    if (entry.ready) {
        if (entry.destArch >= 0) {
            std::cout << "Committing x" << entry.destArch << " = " << entry.value << std::endl;
            cpu.write(entry.destArch, entry.value);
            
            // Clear RAT if this ROB entry is still mapped
            if (rat.get(entry.destArch) == head) {
                std::cout << "Clearing RAT for x" << entry.destArch << std::endl;
                rat.set(entry.destArch, -1);
                
                // Broadcast again to ensure all dependencies are resolved
                cdb.broadcast(head, entry.value);
            }
        }
        
        // Commit complete, free ROB entry
        entry.ready = false;
        head = (head + 1) % buffer.size();
        --count;
    }
}

