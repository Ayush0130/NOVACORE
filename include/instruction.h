#pragma once
#include <string>

enum class OpType { ADD, SUB, MUL, DIV, LD, ST, NOP };

struct Instruction {
    OpType op;
    int rd, rs1, rs2;
    int imm;
    Instruction();
    Instruction(const std::string& line);
};
