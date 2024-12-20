#pragma once
#include "Common.hpp"
#include "Cycle.hpp"
#include "Bus.hpp"
#include "Instruction.hpp"
#include "CPURegister.hpp"
#include "Interrupt.hpp"
#include <functional>

namespace GBCEmu {
class CPUContext {
public:
    CPUContext(Bus& bus, CPURegister& reg, Cycle& cycle, Interrupt& interrupt);

    ~CPUContext() {};
    // 寄存器

    void nop();
    void ld();
    void ldh();
    void inc();
    void dec();
    void ei();
    void di();
    void jp();
    void ret();
    void reti();
    void call();
    void rst();
    void jr();
    void pop();
    void push();
    void add();
    void sub();
    void sbc();
    void adc();
    void pand();
    void pxor();
    void por();
    void cp();
    void cb();
    void rlca();
    void rrca();
    void rla();
    void rra();
    void stop();
    void daa();
    void cpl();
    void scf();
    void ccf();
    void halt();

    Bus& bus_;
    Cycle& cycle_;
    CPURegister& reg_;
    uint8_t curOpcode_;
    uint16_t fetchedData_;
    Instruction curInst_;
    Interrupt& interrupt_;

    bool halt_ = false;
    uint16_t memoDest_;
    bool writeToMemo_ = false;

    // 根据寻址模式准备好数据
    void fetchData();
    void process();
    void handleByAddress(uint16_t addr);

protected:
    uint8_t stackPop();
    uint16_t stackPop16();
    void stackPush(uint8_t val);
    void stackPush16(uint16_t val);
    void go2(uint16_t addr, bool pushPC);
    bool checkCond();
    RegType decodeReg(uint8_t reg);
    std::unordered_map<InstType, std::function<void()>> PROCESSOR_;
};

}