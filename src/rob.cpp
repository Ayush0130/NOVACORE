#include "../include/rob.h"
#include <stdexcept>
#include <iostream>
#include "../include/rat.h"
#include "../include/cdb.h"

ROB::ROB(int capacity)
 : head(0), tail(0), count(0), buffer(capacity) {}

bool ROB::isFull() const { return count == (int)buffer.size(); }

int ROB::allocate(int destArch, bool isfloat) {
    if (isFull()) {
        throw std::runtime_error("ROB full");
        // std::cout << "ROB full" << std::endl;
        // return -1;
    }
    int idx = tail;
    buffer[tail] = {destArch, 0,isfloat, false};
    tail = (tail+1) % buffer.size();
    ++count;
    return idx;
}

void ROB::markReadyFloat(int idx, float value) {
    buffer[idx].fvalue = value;
    buffer[idx].isfloat = true;
    buffer[idx].ready = true;
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
              << " destArch=" << entry.destArch;
    
    if (entry.isfloat) {
        std::cout << " fvalue=" << entry.fvalue << std::endl;
    } else {
        std::cout << " value=" << entry.value << std::endl;
    }
    
    if (entry.ready) {
        if (entry.destArch >= 0) {
            if (!entry.isfloat) {
                std::cout << "Committing x" << entry.destArch << " = " << entry.value << std::endl;
                cpu.write(entry.destArch, entry.value);
                
                // Broadcast again for integer values to ensure dependencies are resolved
                if (rat.get(entry.destArch) == head) {
                    std::cout << "Clearing RAT for x" << entry.destArch << std::endl;
                    rat.set(entry.destArch, -1);
                    cdb.broadcast(head, entry.value);
                }
            } else {
                std::cout << "Committing f" << entry.destArch << " = " << entry.fvalue << std::endl;
                cpu.fwrite(entry.destArch, entry.fvalue);
                
                // Broadcast again for float values
                if (rat.getF(entry.destArch) == head) {
                    std::cout << "Clearing RAT for f" << entry.destArch << std::endl;
                    rat.setF(entry.destArch, -1);
                    cdb.broadcastFloat(head, entry.fvalue);
                }
            }
        }
        
        // Commit complete, free ROB entry
        entry.ready = false;
        head = (head + 1) % buffer.size();
        --count;
    }
}

