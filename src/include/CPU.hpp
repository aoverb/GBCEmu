// src/CPU.h
#pragma once
#include "Common.hpp"
#include "Bus.hpp"
#include "Instruction.hpp"
#include <functional>

namespace GBCEmu {

typedef struct {
    // 寄存器
    uint8_t a_, f_;
    uint8_t b_, c_;
    uint8_t d_, e_;
    uint8_t h_, l_;
    uint16_t sp_, pc_;

    uint8_t curOpcode_;
    uint16_t fetchedData_;
    Instruction curInst_;
    bool halt_;
    bool interruptEnabled_;
    uint8_t getZFlag()
    {
        return getBit(f_, 7);
    }
    uint8_t getCFlag()
    {
        return getBit(f_, 4);
    }
    uint8_t getHFlag()
    {
        return getBit(f_, 5);
    }
    uint8_t getNFlag()
    {
        return getBit(f_, 6);
    }
    uint16_t readReg(RegType reg)
    {
        switch(reg) {
            case RegType::A: return a_;
            case RegType::B: return b_;
            case RegType::C: return c_;
            case RegType::D: return d_;
            case RegType::E: return e_;
            case RegType::F: return f_;
            case RegType::H: return h_;
            case RegType::L: return l_;
            case RegType::SP: return sp_;
            case RegType::PC: return pc_;
        }    
    }

    void writeReg(RegType reg, uint16_t val)
    {
        switch(reg) {
            case RegType::A: a_ = val & 0xFF; return;
            case RegType::B: b_ = val & 0xFF; return;
            case RegType::C: c_ = val & 0xFF; return;
            case RegType::D: d_ = val & 0xFF; return;
            case RegType::E: e_ = val & 0xFF; return;
            case RegType::F: f_ = val & 0xFF; return;
            case RegType::H: h_ = val & 0xFF; return;
            case RegType::L: l_ = val & 0xFF; return;
            case RegType::SP: sp_ = val; return;
            case RegType::PC: pc_ = val; return;
        }    
    }

} CPUContext;

using ProcFun = std::function<void(CPUContext&)>;

class CPU {
public:
    CPU(Bus& bus) : bus_(bus) {
        reset();
    }
    ~CPU();

    bool step();
    CPUContext context_;

protected:
    Bus& bus_;
    // 私有方法
    void reset();
    void fetchInst();
    void fetchData();
    ProcFun getProcessor(InstType& instType);
    void execute();
};
}