// src/CPU.cpp
#include "CPU.hpp"
#include <iostream>

namespace GBCEmu {
// 构造函数
CPU::CPU() : exitFlag(false) {
    reset();
}

// 析构函数
CPU::~CPU() {}

// 重置 CPU
void CPU::reset()
{
    A = F = 0;
    B = C = D = E = H = L = 0;
    SP = 0xFFFE; // 初始化堆栈指针
    PC = 0x0100; // 游戏开始执行地址
}

// 执行下一条指令
bool CPU::step()
{
    std::cout << "not implemented" << "\n";
    return false;
}

bool CPU::shouldExit() const
{
    return exitFlag;
}
}