// src/CPU.h
#pragma once
#include "CPUContext.hpp"

namespace GBCEmu {
class CPU {
public:
    CPU(Bus& bus, CPURegister& reg, Cycle& cycle) : context_(bus, reg, cycle) {
        reset();
    }
    ~CPU();

    bool step();
    CPUContext context_;
    Cycle cycle_;

protected:
    // 私有方法
    void reset();
    void fetchInst();
    void execute();
};
}