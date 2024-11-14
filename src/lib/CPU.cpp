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
    if (context.writeToMemo_) {
        if (context.curInst_.reg2 == RegType::AF) {
            context.bus_.write16(context.memoDest_, context.fetchedData_);
        } else {
            context.bus_.write(context.memoDest_, context.fetchedData_);
        }
        context.writeToMemo_ = false;
        return;
    }

    if (context.curInst_.mode == AddrMode::HL_SPR) {
        uint8_t hFlag = (context.readReg(context.curInst_.reg1) & 0xF) +
            (context.readReg(context.curInst_.reg2) & 0xF) >= 0x10;
        uint8_t cFlag = (context.readReg(context.curInst_.reg1) & 0xFF) +
            (context.readReg(context.curInst_.reg2) & 0xFF) >= 0x100;
        context.setFlags(0, 0, hFlag, cFlag);
        context.writeReg(context.curInst_.reg1, context.readReg(context.curInst_.reg2) +
            static_cast<int8_t>(context.fetchedData_));
        return;
    }
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

static void xor(CPUContext& context)
{
    context.a_ ^= context.fetchedData_ & 0xFF;
    context.setFlags(context.a_, 0, 0, 0);
}

const std::unordered_map<InstType, ProcFun> PROCESSOR = {
    {InstType::NOP, nop},
    {InstType::LD, ld},
    {InstType::JP, jp},
    {InstType::DI, di},
    {InstType::XOR, xor}
};
}


// 根据寻址模式准备好数据
void CPUContext::fetchData()
{
    FUNC_TRACE("CPU::fetchData");
    uint16_t low, high;
    switch(curInst_.mode) {
        case AddrMode::IMP: return;
        case AddrMode::R:
            fetchedData_ = readReg(curInst_.reg1);
            return;
        case AddrMode::R_R:
            fetchedData_ = readReg(curInst_.reg2);
            return;
        case AddrMode::MR_R:
            fetchedData_ = readReg(curInst_.reg2);
            memoDest_ = bus_.read(readReg(curInst_.reg1));
            writeToMemo_ = true;
            return;
        case AddrMode::MR:
            memoDest_ = readReg(curInst_.reg1);
            writeToMemo_ = true;
            fetchedData_ = readReg(curInst_.reg1);
            return;
        case AddrMode::MR_D8:
            fetchedData_ = bus_.read(pc_);
            ++pc_;
            memoDest_ = readReg(curInst_.reg1);
            writeToMemo_ = true;
            return;
        case AddrMode::MC_R:
            fetchedData_ = readReg(curInst_.reg2);
            memoDest_ = readReg(RegType::C) | 0xFF00;
            writeToMemo_ = true;
            return;
        case AddrMode::R_MC:
            fetchedData_ = bus_.read(readReg(RegType::C) | 0xFF00);
            return;
        case AddrMode::R_HLI:
            fetchedData_ = bus_.read(readReg(RegType::HL));
            writeReg(RegType::HL, readReg(RegType::HL) + 1);
            return;
        case AddrMode::R_HLD:
            fetchedData_ = bus_.read(readReg(RegType::HL));
            writeReg(RegType::HL, readReg(RegType::HL) - 1);
            return;
        case AddrMode::HLI_R:
            fetchedData_ = readReg(curInst_.reg2);
            memoDest_ = readReg(RegType::HL);
            writeToMemo_ = true;
            writeReg(RegType::HL, readReg(RegType::HL) + 1);
            return;
        case AddrMode::HLD_R:
            fetchedData_ = readReg(curInst_.reg2);
            memoDest_ = readReg(RegType::HL);
            writeToMemo_ = true;
            writeReg(RegType::HL, readReg(RegType::HL) - 1);
            return;
        case AddrMode::R_D8:
            fetchedData_ = bus_.read(pc_);
            // emu_.cycle(1);
            ++pc_;
            return;
        case AddrMode::R_A8:
            fetchedData_ = bus_.read(bus_.read(pc_));
            // emu_.cycle(1);
            ++pc_;
            return;
        case AddrMode::A8_R:
            memoDest_ = bus_.read(pc_);
            fetchedData_ = readReg(curInst_.reg2);
            writeToMemo_ = true;
            // emu_.cycle(1);
            ++pc_;
            return;
        case AddrMode::R_HA8:
            fetchedData_ = 0xFF00 | bus_.read(bus_.read(pc_));
            // emu_.cycle(1);
            ++pc_;
            return;
        case AddrMode::HA8_R:
            memoDest_ = 0xFF00 | bus_.read(pc_);
            fetchedData_ = readReg(curInst_.reg2);
            writeToMemo_ = true;
            // emu_.cycle(1);
            ++pc_;
            return;
        case AddrMode::A16_R:
            low = bus_.read(pc_);
            // emu_.cycle(1);
            high = bus_.read(pc_ + 1);
            // emu_.cycle(1);
            memoDest_ = low | (high << 8);
            writeToMemo_ = true;
            fetchedData_ = readReg(curInst_.reg2);
            pc_ += 2;
            return;
        case AddrMode::R_A16:
            low = bus_.read(pc_);
            // emu_.cycle(1);
            high = bus_.read(pc_ + 1);
            // emu_.cycle(1);
            fetchedData_ = bus_.read(low | (high << 8));
            pc_ += 2;
            return;
        case AddrMode::R_D16:
        case AddrMode::D16:
            low = bus_.read(pc_);
            // emu_.cycle(1);
            high = bus_.read(pc_ + 1);
            // emu_.cycle(1);
            fetchedData_ = low | (high << 8);
            pc_ += 2;
            return;
        default:
            std::cout << "UNKNOWN ADDRESSING MODE \n";
            exit(-6);
    }
}


// 析构函数
CPU::~CPU() {}

// 重置 CPU
void CPU::reset()
{
    context_.a_ = 0x01;
    context_.f_ = 0;
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
        context_.curOpcode_ = context_.bus_.read(context_.pc_++);
        context_.curInst_ = ::std::move(getInstructionByOpCode(context_.curOpcode_));
        std::cout << context_.pc_ - 1 << "\t Opcode:" << inst_lookup[static_cast<int>(context_.curInst_.type)] <<
            " " << (int)context_.bus_.read(context_.pc_ - 1) <<
            " " << (int)context_.bus_.read(context_.pc_) <<
            " " << (int)context_.bus_.read(context_.pc_ + 1) << "\t"
            << "A " << (int)context_.a_ << "\t"
            << "B " << (int)context_.b_ << "\t"
            << "C " << (int)context_.c_ << "\t"
            << "D " << (int)context_.d_ << "\t"
            << "E " << (int)context_.e_ << "\t"
            << "H " << (int)context_.h_ << "\t"
            << "L " << (int)context_.l_ << "\t"
            << "SP " << context_.sp_ << "\n";
        
    } catch (std::exception ex) {
        std::cerr << "CPU::fetchInst catches exception: " << ex.what() << "\n";
    } catch (...) {
        std::cerr << "CPU::fetchInst catches exception!\n";
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
        context_.fetchData();
        execute();
    }
    return true;
}
}