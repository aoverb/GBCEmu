// src/CPU.cpp
#include "CPU.hpp"
#include <iostream>
#include <utility>

namespace GBCEmu {

namespace Processor {

static bool checkCond(CPUContext& context)
{
    bool z = context.getZFlag();
    bool c = context.getCFlag();

    switch (context.curInst_.cond) {
        case CondType::NONE: return true;
        case CondType::C: return c;
        case CondType::NC: return !c;
        case CondType::Z: return z;
        case CondType::NZ: return !z;
    }

    return false;
}

static void nop(CPUContext& context)
{
    return;
}

static void ld(CPUContext& context)
{
    context.writeReg(context.curInst_.reg1, context.fetchedData_);
}

static void di(CPUContext& context)
{
    context.interruptEnabled_ = false;
}

static void jp(CPUContext& context)
{
    if (checkCond(context)) {
        context.pc_ = context.fetchedData_;
        // emu_.cycle(1);
    }
}

const std::unordered_map<InstType, ProcFun> PROCESSOR = {
    {InstType::NOP, nop},
    {InstType::LD, ld},
    {InstType::JP, jp},
    {InstType::DI, di},
};
}


// 析构函数
CPU::~CPU() {}

// 重置 CPU
void CPU::reset()
{
    context_.a_ = context_.f_ = 0;
    context_.b_ = context_.c_ = context_.d_ = context_.e_ = context_.h_ = context_.l_ = 0;
    context_.sp_ = 0xFFFE; // 初始化堆栈指针
    context_.pc_ = 0x0100; // 游戏开始执行地址
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
        context_.curOpcode_ = bus_.read(context_.pc_);
        context_.curInst_ = ::std::move(getInstructionByOpCode(context_.curOpcode_));
        std::cout << context_.pc_ << "\t Opcode:" << inst_lookup[static_cast<int>(context_.curInst_.type)] <<
            " " << (int)bus_.read(context_.pc_) <<
            " " << (int)bus_.read(context_.pc_ + 1) <<
            " " << (int)bus_.read(context_.pc_ + 2) << "\t"
            << "A " << (int)context_.a_ << "\t"
            << "B " << (int)context_.b_ << "\t"
            << "C " << (int)context_.c_ << "\t"
            << "D " << (int)context_.d_ << "\t"
            << "SP " << context_.sp_ << "\n";
        ++context_.pc_;
        
    } catch (std::exception ex) {
        std::cerr << "CPU::fetchInst catches exception: " << ex.what() << "\n";
    } catch (...) {
        std::cerr << "CPU::fetchInst catches exception!\n";
    }
}

// 根据寻址模式准备好数据
void CPU::fetchData()
{
    FUNC_TRACE("CPU::fetchData");
    uint16_t low, high;
    switch(context_.curInst_.mode) {
        case AddrMode::IMP: return;
        case AddrMode::R:
            context_.fetchedData_ = context_.readReg(context_.curInst_.reg1);
            return;
        case AddrMode::R_D8:
            context_.fetchedData_ = bus_.read(context_.pc_);
            // emu_.cycle(1);
            ++context_.pc_;
            return;
        case AddrMode::R_D16:
            low = bus_.read(context_.pc_);
            // emu_.cycle(1);
            high = bus_.read(context_.pc_ + 1);
            // emu_.cycle(1);
            context_.fetchedData_ = low | (high << 8);
            context_.pc_ += 2;
            return;
        case AddrMode::D16:
            low = bus_.read(context_.pc_);
            // emu_.cycle(1);
            high = bus_.read(context_.pc_ + 1);
            // emu_.cycle(1);
            context_.fetchedData_ = low | (high << 8);
            context_.pc_ += 2;
            return;
        case AddrMode::R_R:
            
        default:
            std::cout << "UNKNOWN ADDRESSING MODE \n";
            exit(-6);
    }
}

ProcFun CPU::getProcessor(InstType& instType)
{
    auto it = Processor::PROCESSOR.find(instType);
    if(it != Processor::PROCESSOR.end()) {
        return it->second;
    }
    throw std::runtime_error("failed to find processor of instType:" + static_cast<int>(instType));
}

void CPU::execute()
{
    FUNC_TRACE("CPU::execute");
    try {
        getProcessor(context_.curInst_.type)(context_);
    } catch (std::exception ex) {
        std::cerr << "CPU::execute catches exception: " << ex.what() << "\n";
    }
    
}

// 执行下一条指令
bool CPU::step()
{
    if (!context_.halt_) {
        fetchInst();
        fetchData();
        execute();
    }
    return true;
}
}