#include "../include/cdb.h"

void CDB::broadcast(int tag, int value) {
    for (auto& l : listeners) l(tag, value);
}

void CDB::subscribe(std::function<void(int,int)> listener) {
    listeners.push_back(listener);
}
