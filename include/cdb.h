#pragma once
#include <functional>
#include <vector>

class CDB {
public:
    void broadcast(int tag, int value);
    void subscribe(std::function<void(int,int)> listener);
private:
    std::vector<std::function<void(int,int)>> listeners;
};
