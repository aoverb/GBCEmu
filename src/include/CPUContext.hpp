#pragma once
#include "Common.hpp"
#include "Cycle.hpp"
#include "Bus.hpp"
#include "Instruction.hpp"
#include "CPURegister.hpp"
#include <functional>

namespace GBCEmu {

enum class InterruptType {
    VBLANK = 1,
    LCD_STAT = 2,
    TIMER = 4,
    SERIAL = 8,
    JOYPAD = 16
};

class CPUContext {
public:
    CPUContext(Bus& bus, CPURegister& reg, Cycle& cycle);

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
    void and();
    void xor();
    void or();
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
    uint8_t intFlag_ = false;
    bool halt_ = false;
    bool interruptEnabled_ = false;
    bool enablingIME_ = false;
    uint16_t memoDest_;
    bool writeToMemo_ = false;

    // 根据寻址模式准备好数据
    void fetchData();
    void process();
    void requestInterrupt(InterruptType interrupt);
    void handleInterrupt();
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
    uint8_t getIntFlags() {
        return intFlag_;
    }
    void setIntFlag(uint8_t flag) {
        intFlag_ = flag;
    }
};

}