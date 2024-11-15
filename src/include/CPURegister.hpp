#pragma once
#include "Common.hpp"
#include "Instruction.hpp"
#include <functional>

namespace GBCEmu {
class CPURegister {
public:
    CPURegister() {};
    ~CPURegister() {};
    uint8_t a_, f_;
    uint8_t b_, c_;
    uint8_t d_, e_;
    uint8_t h_, l_;
    uint16_t sp_, pc_;
    uint8_t ie_;

    void setFlags(int z, int n, int h, int c);

    uint8_t getZFlag();
    uint8_t getCFlag();
    uint8_t getHFlag();
    uint8_t getNFlag();
    uint16_t readReg(RegType reg);
    void writeReg(RegType reg, uint16_t val);
};

}