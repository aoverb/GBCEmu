// src/CPU.h
#pragma once
#include "CPUContext.hpp"

namespace GBCEmu {
class CPU {
public:
    CPU(Bus& bus, CPURegister& reg) : context_(bus, reg) {
        reset();
    }
    ~CPU();

    bool step();
    CPUContext context_;

protected:
    // 私有方法
    void reset();
    void fetchInst();
    ProcFun getProcessor(InstType& instType);
    void execute();
};
}