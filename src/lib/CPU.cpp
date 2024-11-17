// src/CPU.cpp
#include "CPU.hpp"
#include <iostream>
#include <utility>

namespace GBCEmu {


namespace Processor {

}

// 析构函数
CPU::~CPU() {}

// 重置 CPU
void CPU::reset()
{
    context_.reg_.a_ = 0x01;
    context_.reg_.f_ = 0;
    context_.reg_.b_ = context_.reg_.c_ = context_.reg_.d_ = context_.reg_.e_ = context_.reg_.h_ = context_.reg_.l_ = 0;
    context_.reg_.b_ = 1;
    context_.reg_.sp_ = 0x0; // 初始化堆栈指针
    context_.reg_.pc_ = 0x0100; // 游戏开始执行地址
    context_.halt_ = false;
    context_.interruptEnabled_ = true;
}


char *inst_lookup[] = {
    "<NONE>",
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RLCA",
    "ADD",
    "RRCA",
    "STOP",
    "RLA",
    "JR",
    "RRA",
    "DAA",
    "CPL",
    "SCF",
    "CCF",
    "HALT",
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "XOR",
    "OR",
    "CP",
    "POP",
    "JP",
    "PUSH",
    "RET",
    "CB",
    "CALL",
    "RETI",
    "LDH",
    "JPHL",
    "DI",
    "EI",
    "RST",
    "IN_ERR",
    "IN_RLC", 
    "IN_RRC",
    "IN_RL", 
    "IN_RR",
    "IN_SLA", 
    "IN_SRA",
    "IN_SWAP", 
    "IN_SRL",
    "IN_BIT", 
    "IN_RES", 
    "IN_SET"
};


void CPU::fetchInst()
{
    FUNC_TRACE("CPU::fetchInst");
    try {
        context_.curOpcode_ = context_.bus_.read(context_.reg_.pc_++);
        context_.curInst_ = ::std::move(getInstructionByOpCode(context_.curOpcode_));
        std::cout << context_.reg_.pc_ - 1 << "\t Opcode:" << inst_lookup[static_cast<int>(context_.curInst_.type)] <<
            " " << (int)context_.bus_.read(context_.reg_.pc_ - 1) <<
            " " << (int)context_.bus_.read(context_.reg_.pc_) <<
            " " << (int)context_.bus_.read(context_.reg_.pc_ + 1) << "\t"
            << "A " << (int)context_.reg_.a_ << "\t"
            << "B " << (int)context_.reg_.b_ << "\t"
            << "C " << (int)context_.reg_.c_ << "\t"
            << "D " << (int)context_.reg_.d_ << "\t"
            << "E " << (int)context_.reg_.e_ << "\t"
            << "H " << (int)context_.reg_.h_ << "\t"
            << "L " << (int)context_.reg_.l_ << "\t"
            << "F " << (int)context_.reg_.f_ << "\t"
            << "SP " << context_.reg_.sp_ << "\n";
        
    } catch (std::exception ex) {
        std::cerr << "CPU::fetchInst catches exception: " << ex.what() << "\n";
    } catch (...) {
        std::cerr << "CPU::fetchInst catches exception!\n";
    }
}

void CPU::execute()
{
    FUNC_TRACE("CPU::execute");
    try {
        context_.process();
    } catch (std::exception ex) {
        std::cerr << "CPU::execute catches exception: " << ex.what() << "\n";
    }
    
}

// 执行下一条指令
bool CPU::step()
{
    if (!context_.halt_) {
        fetchInst();
        context_.fetchData();
        execute();
    }
    return true;
}
}