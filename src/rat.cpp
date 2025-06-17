#include "../include/rat.h"
RAT::RAT(int numArch) : table(numArch, -1), ftable(numArch, -1) {}
int RAT::get(int arch) const {
     return table[arch]; 
    }
void RAT::set(int arch, int phys) { 
    table[arch] = phys; 
}

int RAT::getF(int arch) const {
    return ftable[arch];
}
void RAT::setF(int arch, int phys) {
    ftable[arch] = phys;
}
