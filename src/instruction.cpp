#include "../include/instruction.h"
#include <sstream>
#include <unordered_map>
#include <regex>
#include <stdexcept>
#include <iostream>

Instruction::Instruction() : op(OpType::NOP), rd(0), rs1(0), rs2(0), imm(0) {}

static int parseReg(const std::string& tok) {
    if (tok.empty()) throw std::invalid_argument("Empty register name");
    
    // Allow both 'x' and 'f' register prefixes
    if (tok[0] != 'x' && tok[0] != 'f') {
        throw std::invalid_argument("Invalid register prefix: " + tok);
    }
    
    return std::stoi(tok.substr(1));
}

Instruction::Instruction(const std::string& line) : rd(0), rs1(0), rs2(0), imm(0) {
    std::istringstream iss(line);
    std::string opc;
    iss >> opc;
    static std::unordered_map<std::string,OpType> map = {
        {"add",OpType::ADD},
         {"sub",OpType::SUB},
        {"mul",OpType::MUL}, 
        {"div",OpType::DIV},
        {"lw",OpType::LD},  
         {"sw",OpType::ST},
         {"fadd",OpType::FADD},
    };
    auto it = map.find(opc);
    if (it==map.end()) { 
        op = OpType::NOP; 
        std::invalid_argument("Invalid Opcode");
        std::cout<<"Invalid Opcode: "<<opc<<"\n";
        return; 
    }
    std::cout<<"debug opcode: "<<opc<<"\n";
    op = it->second;
    std::string rest;
    getline(iss, rest);
    for (char& c: rest) if (c==',') c=' ';
    std::istringstream args(rest);

    if (op==OpType::ADD||op==OpType::SUB||op==OpType::MUL||op==OpType::DIV || op==OpType::FADD) {
        std::cout<<"debug args: "<<rest<<"\n";
        std::string a,b,c;
        args>>a>>b>>c;
        rd=parseReg(a); rs1=parseReg(b); rs2=parseReg(c);
    } else {
        // lw x1, 4(x2)  or  sw x3, -8(x4)
        std::string rd_s, offreg;
        args>>rd_s>>offreg;
        std::smatch m;
        std::regex re(R"(([-]?\d+)\((x\d+)\))");
        if (!std::regex_match(offreg,m,re)) throw std::invalid_argument("Bad mem operand");
        imm = std::stoi(m[1].str());
        std::cout<<"debug imm: "<<imm<<"\n";
        std::cout<<"debug reg: "<<m[2].str()<<"\n";
        rs1 = parseReg(m[2].str());
        if (op==OpType::LD) rd = parseReg(rd_s);
        else rs2 = parseReg(rd_s);
    }
}
