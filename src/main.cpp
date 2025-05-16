#include <iostream>
#include <fstream>
#include <vector>
#include "../include/instruction.h"
#include "../include/simulation.h"

int main(int argc, char* argv[]) {
    if (argc<2) {
        std::cerr<<"Usage: "<<argv[0]<<" program.asm\n";
        return 1;
    }
    std::ifstream in(argv[1]);
    if (!in) { std::cerr<<"Cannot open "<<argv[1]<<"\n"; return 1; }
    std::vector<Instruction> prog;
    std::string line;
    while (std::getline(in,line)) {
        auto p = line.find('#');
        if (p!=std::string::npos) line=line.substr(0,p);
        if (line.empty()) continue;
        prog.emplace_back(line);
        //std::cout<<line<<"\n";
    }
    Simulation sim(prog, 1024, 2, 4);
    sim.run();
    return 0;
}
