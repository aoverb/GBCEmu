// src/CPU.cpp
#include "CPU.hpp"
#include <iostream>
#include <utility>

namespace GBCEmu {


namespace Processor {

static bool checkCond(CPUContext& context)
{
    bool z = context.reg_.getZFlag();
    bool c = context.reg_.getCFlag();

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
        if (is16bitReg(context.curInst_.reg2)) {
            context.bus_.write16(context.memoDest_, context.fetchedData_);
        } else {
            context.bus_.write(context.memoDest_, context.fetchedData_);
        }
        context.writeToMemo_ = false;
        return;
    }

    if (context.curInst_.mode == AddrMode::HL_SPR) {
        uint8_t hFlag = (context.reg_.readReg(context.curInst_.reg1) & 0xF) +
            (context.reg_.readReg(context.curInst_.reg2) & 0xF) >= 0x10;
        uint8_t cFlag = (context.reg_.readReg(context.curInst_.reg1) & 0xFF) +
            (context.reg_.readReg(context.curInst_.reg2) & 0xFF) >= 0x100;
        context.reg_.setFlags(0, 0, hFlag, cFlag);
        context.reg_.writeReg(context.curInst_.reg1, context.reg_.readReg(context.curInst_.reg2) +
            static_cast<int8_t>(context.fetchedData_));
        return;
    }
    context.reg_.writeReg(context.curInst_.reg1, context.fetchedData_);
}

static void inc(CPUContext& context)
{
    if (is16bitReg(context.curInst_.reg1)) {
        // ... emu
    }
    uint16_t val = context.reg_.readReg(context.curInst_.reg1) + 1;
    if (context.writeToMemo_) {
        val = context.bus_.read(context.fetchedData_) & 0xFF + 1;
        context.bus_.write(context.fetchedData_, val);
    } else {
        context.reg_.writeReg(context.curInst_.reg1, val);
    }

    if (context.curOpcode_ & 0x03 == 0x03) {
        return;
    }

    context.reg_.setFlags(val == 0, 0, (val & 0x0F) == 0, -1);
}

static void dec(CPUContext& context)
{
    if (is16bitReg(context.curInst_.reg1)) {
        // ... emu
    }
    uint16_t val = context.reg_.readReg(context.curInst_.reg1) - 1;
    if (context.writeToMemo_) {
        val = context.bus_.read(context.fetchedData_) & 0xFF - 1;
        context.bus_.write(context.fetchedData_, val);
    } else {
        context.reg_.writeReg(context.curInst_.reg1, val);
    }

    if (context.curOpcode_ & 0x0B == 0x0B) {
        return;
    }

    context.reg_.setFlags(val == 0, 1, (val & 0x0F) == 0x0F, -1);
}

static uint8_t stackPop(CPUContext& context)
{
    return context.bus_.read(context.reg_.sp_++);
}

static uint16_t stackPop16(CPUContext& context)
{
    uint16_t lo;
    lo = stackPop(context);
    uint16_t hi;
    hi = stackPop(context);
    return (hi << 8) | lo;
}

static void stackPush(CPUContext& context, uint8_t val)
{
    context.bus_.write(--context.reg_.sp_, val);
}

static void stackPush16(CPUContext& context, uint16_t val)
{
    stackPush(context, (val >> 8) & 0xFF);
    stackPush(context, val & 0xFF);
}

static void di(CPUContext& context)
{
    context.interruptEnabled_ = false;
}

static void go2(CPUContext& context, uint16_t addr, bool pushPC)
{
    if (checkCond(context)) {
        if (pushPC) {
            stackPush16(context, context.reg_.pc_);
        }
        context.reg_.pc_ = addr;
        // emu_.cycle(1);
    }
}

static void jp(CPUContext& context)
{
    go2(context, context.fetchedData_, false);
}

static void ret(CPUContext& context)
{
    if (context.curInst_.cond != CondType::NONE) {
        // emu_cycle
    }

    if (checkCond(context)) {
        uint16_t lo;
        lo = stackPop(context);
        uint16_t hi;
        hi = stackPop(context);
        uint16_t res = (hi << 8) | lo;
        context.reg_.pc_ = res;
    }
}

static void reti(CPUContext& context)
{
    context.interruptEnabled_ = true;
    ret(context);
}

static void call(CPUContext& context)
{
    go2(context, context.fetchedData_, true);
}

static void rst(CPUContext& context)
{
    go2(context, context.curInst_.param, true);
}

static void jr(CPUContext& context)
{
    uint16_t addr = context.reg_.pc_ + static_cast<int8_t>(context.fetchedData_ & 0xFF);
    go2(context, addr, true);
}

static void xor(CPUContext& context)
{
    context.reg_.a_ ^= context.fetchedData_ & 0xFF;
    context.reg_.setFlags(context.reg_.a_, 0, 0, 0);
}

static void pop(CPUContext& context)
{
    uint16_t lo;
    lo = stackPop(context);
    uint16_t hi;
    hi = stackPop(context);
    uint16_t res = (hi << 8) | lo;
    context.reg_.writeReg(context.curInst_.reg1,
        res & (context.curInst_.reg1 == RegType::AF ? 0xFFF0 : 0xFFFF));
}

static void push(CPUContext& context)
{
    uint16_t hi;
    hi = (context.reg_.readReg(context.curInst_.reg1) >> 8) & 0xFF;
    stackPush(context, hi);
    uint16_t lo;
    lo = hi = context.reg_.readReg(context.curInst_.reg1) & 0xFF;
    stackPush(context, lo);
}

const std::unordered_map<InstType, ProcFun> PROCESSOR = {
    {InstType::NOP, nop},
    {InstType::LD, ld},
    {InstType::JP, jp},
    {InstType::JR, jr},
    {InstType::CALL, call},
    {InstType::DI, di},
    {InstType::XOR, xor},
    {InstType::POP, pop},
    {InstType::PUSH, push},
    {InstType::RET, ret},
    {InstType::RETI, reti},
    {InstType::RST, rst},
    {InstType::INC, inc},
    {InstType::DEC, dec}
};
}

// 析构函数
CPU::~CPU() {}

// 重置 CPU
void CPU::reset()
{
    context_.reg_.a_ = 0x01;
    context_.reg_.f_ = 0;
    context_.reg_.b_ = context_.reg_.c_ = context_.reg_.d_ = context_.reg_.e_ = context_.reg_.h_ = context_.reg_.l_ = 0;
    context_.reg_.sp_ = 0xFFFE; // 初始化堆栈指针
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
            << "SP " << context_.reg_.sp_ << "\n";
        
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