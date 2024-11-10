// src/CPU.cpp
#include "CPU.hpp"
#include <iostream>

// 构造函数
CPU::CPU(Memory& mem) : memory(mem), exitFlag(false) {
    reset();
}

// 析构函数
CPU::~CPU() {}

// 重置 CPU
void CPU::reset() {
    A = F = 0;
    B = C = D = E = H = L = 0;
    SP = 0xFFFE; // 初始化堆栈指针
    PC = 0x0100; // 游戏开始执行地址
}

// 执行下一条指令
void CPU::executeNextInstruction() {
    // 示例：简单读取一个字节并打印
    uint8_t opcode = memory.readByte(PC);
    std::cout << "instructuion: 0x" << std::hex << static_cast<int>(opcode) << " at PC: " << PC << "\n";

    // 简单模拟退出指令（假设 0x00 是退出指令）
    if (opcode == 0x90) {
        exitFlag = true;
        return;
    }

    // 增加 PC
    PC += 1;

    // TODO: 实现指令解码和执行
}

bool CPU::shouldExit() const {
    return exitFlag;
}