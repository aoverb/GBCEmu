// src/CPU.h
#pragma once
#include "CPUContext.hpp"

namespace GBCEmu {
class CPU {
public:
    CPU(Bus& bus, CPURegister& reg, Cycle& cycle, Interrupt& interrupt) :
        context_(bus, reg, cycle, interrupt), cycle_(cycle), interrupt_(interrupt) {
        reset();
    }
    ~CPU();

    bool step();
    CPUContext context_;
    Cycle& cycle_;
    Interrupt& interrupt_;

protected:
    // 私有方法
    void inst_to_str(char *str);
    void reset();
    void fetchInst();
    void execute();
};
}