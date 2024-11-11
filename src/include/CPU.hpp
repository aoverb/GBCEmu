// src/CPU.h
#pragma once
#include "Common.hpp"
#include "Bus.hpp"
#include "Instruction.hpp"

namespace GBCEmu {
class CPU {
public:
    CPU(Bus& bus) : bus_(bus) {
        reset();
    }
    ~CPU();

    bool step();

protected:
    // 寄存器
    uint8_t a_, f_;
    uint8_t b_, c_;
    uint8_t d_, e_;
    uint8_t h_, l_;
    uint16_t sp_, pc_;

    uint8_t curOpcode_;
    uint16_t fetchedData_;

    Bus& bus_;
    Instruction curInst_;

    bool halt_;

    // 私有方法
    void reset();
    void fetchInst();
    void fetchData();
    uint16_t readReg(RegType reg);
    void execute();
};
}