// src/CPU.h
#ifndef CPU_H
#define CPU_H

#include "Memory.hpp"

class CPU {
public:
    CPU(Memory& memory);
    ~CPU();

    void executeNextInstruction();
    bool shouldExit() const;

private:
    // 寄存器
    uint8_t A, F;
    uint8_t B, C;
    uint8_t D, E;
    uint8_t H, L;
    uint16_t SP, PC;

    Memory& memory;
    bool exitFlag;

    // 私有方法
    void reset();
    void fetchAndExecute();
};

#endif // CPU_H