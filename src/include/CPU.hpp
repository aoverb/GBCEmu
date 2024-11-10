// src/CPU.h
#pragma once

namespace GBCEmu {
class CPU {
public:
    CPU();
    ~CPU();

    bool step();
    bool shouldExit() const;

protected:
    // 寄存器
    uint8_t A, F;
    uint8_t B, C;
    uint8_t D, E;
    uint8_t H, L;
    uint16_t SP, PC;

    bool exitFlag;

    // 私有方法
    void reset();
    void fetchAndExecute();
};
}