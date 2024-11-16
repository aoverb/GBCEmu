#pragma once
#include "Common.hpp"
#include "Bus.hpp"
#include "Instruction.hpp"
#include "CPURegister.hpp"
#include <functional>

namespace GBCEmu {

class CPUContext {
public:
    CPUContext(Bus& bus, CPURegister& reg);

    ~CPUContext() {};
    // 寄存器

    void nop();
    void ld();
    void inc();
    void dec();
    void di();
    void jp();
    void ret();
    void reti();
    void call();
    void rst();
    void jr();
    void xor();
    void pop();
    void push();
    void add();
    void sub();
    void sbc();
    void adc();

    Bus& bus_;
    CPURegister& reg_;
    uint8_t curOpcode_;
    uint16_t fetchedData_;
    Instruction curInst_;
    bool halt_;
    bool interruptEnabled_;
    uint16_t memoDest_;
    bool writeToMemo_ = false;

    // 根据寻址模式准备好数据
    void fetchData();
    void process();

protected:
    uint8_t stackPop();
    uint16_t stackPop16();
    void stackPush(uint8_t val);
    void stackPush16(uint16_t val);
    void go2(uint16_t addr, bool pushPC);
    bool checkCond();
    std::unordered_map<InstType, std::function<void()>> PROCESSOR_;

};

}