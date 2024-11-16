#include "CPUContext.hpp"
namespace GBCEmu {

// 根据寻址模式准备好数据

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

void CPUContext::fetchData()
{
    FUNC_TRACE("CPU::fetchData");
    uint16_t low, high;
    switch(curInst_.mode) {
        case AddrMode::IMP: return;
        case AddrMode::R:
            fetchedData_ = reg_.readReg(curInst_.reg1);
            return;
        case AddrMode::R_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            return;
        case AddrMode::MR_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            memoDest_ = bus_.read(reg_.readReg(curInst_.reg1));
            writeToMemo_ = true;
            return;
        case AddrMode::MR:
            memoDest_ = reg_.readReg(curInst_.reg1);
            writeToMemo_ = true;
            fetchedData_ = reg_.readReg(curInst_.reg1);
            return;
        case AddrMode::D8:
            fetchedData_ = bus_.read(reg_.pc_);
            ++reg_.pc_;
            return;
        case AddrMode::MR_D8:
            fetchedData_ = bus_.read(reg_.pc_);
            ++reg_.pc_;
            memoDest_ = reg_.readReg(curInst_.reg1);
            writeToMemo_ = true;
            return;
        case AddrMode::MC_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            memoDest_ = reg_.readReg(RegType::C) | 0xFF00;
            writeToMemo_ = true;
            return;
        case AddrMode::R_MC:
            fetchedData_ = bus_.read(reg_.readReg(RegType::C) | 0xFF00);
            return;
        case AddrMode::R_HLI:
            fetchedData_ = bus_.read(reg_.readReg(RegType::HL));
            reg_.writeReg(RegType::HL, reg_.readReg(RegType::HL) + 1);
            return;
        case AddrMode::R_HLD:
            fetchedData_ = bus_.read(reg_.readReg(RegType::HL));
            reg_.writeReg(RegType::HL, reg_.readReg(RegType::HL) - 1);
            return;
        case AddrMode::HLI_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            memoDest_ = reg_.readReg(RegType::HL);
            writeToMemo_ = true;
            reg_.writeReg(RegType::HL,reg_.readReg(RegType::HL) + 1);
            return;
        case AddrMode::HLD_R:
            fetchedData_ = reg_.readReg(curInst_.reg2);
            memoDest_ = reg_.readReg(RegType::HL);
            writeToMemo_ = true;
            reg_.writeReg(RegType::HL, reg_.readReg(RegType::HL) - 1);
            return;
        case AddrMode::R_D8:
            fetchedData_ = bus_.read(reg_.pc_);
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::R_A8:
            fetchedData_ = bus_.read(bus_.read(reg_.pc_));
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::A8_R:
            memoDest_ = bus_.read(reg_.pc_);
            fetchedData_ = reg_.readReg(curInst_.reg2);
            writeToMemo_ = true;
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::R_HA8:
            fetchedData_ = 0xFF00 | bus_.read(bus_.read(reg_.pc_));
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::HA8_R:
            memoDest_ = 0xFF00 | bus_.read(reg_.pc_);
            fetchedData_ = reg_.readReg(curInst_.reg2);
            writeToMemo_ = true;
            // emu_.cycle(1);
            ++reg_.pc_;
            return;
        case AddrMode::A16_R:
            low = bus_.read(reg_.pc_);
            // emu_.cycle(1);
            high = bus_.read(reg_.pc_ + 1);
            // emu_.cycle(1);
            memoDest_ = low | (high << 8);
            writeToMemo_ = true;
            fetchedData_ = reg_.readReg(curInst_.reg2);
            reg_.pc_ += 2;
            return;
        case AddrMode::R_A16:
            low = bus_.read(reg_.pc_);
            // emu_.cycle(1);
            high = bus_.read(reg_.pc_ + 1);
            // emu_.cycle(1);
            fetchedData_ = bus_.read(low | (high << 8));
            reg_.pc_ += 2;
            return;
        case AddrMode::R_D16:
        case AddrMode::D16:
            low = bus_.read(reg_.pc_);
            // emu_.cycle(1);
            high = bus_.read(reg_.pc_ + 1);
            // emu_.cycle(1);
            fetchedData_ = low | (high << 8);
            reg_.pc_ += 2;
            return;
        default:
            std::cout << "UNKNOWN ADDRESSING MODE \n";
            exit(-6);
    }
}
}