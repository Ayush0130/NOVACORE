#include "../include/cdb.h"

void CDB::broadcast(int tag, int value) {
    for (auto& l : listeners) l(tag, value,false);
}

void CDB::subscribe(std::function<void(int,int,bool)> listener) {
    listeners.push_back(listener);
}

void CDB::broadcastFloat(int tag, float value) {
    for (auto& l : listeners) l(tag, *reinterpret_cast<int*>(&value),true); // Cast float to int for broadcasting
}
