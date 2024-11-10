// src/Emulator.cpp
#include "Emulator.hpp"
#include <iostream>
#include <fstream>

// 构造函数
Emulator::Emulator() : cpu(memory), memory() {
    // 初始化其他模块
}

// 析构函数
Emulator::~Emulator() {
    // 清理资源
}

bool Emulator::loadROM(const std::string& path) {
    std::ifstream romFile(path, std::ios::binary | std::ios::ate);
    if (!romFile.is_open()) {
        std::cerr << "Failed to open ROM" << path << "\n";
        return false;
    }

    std::streamsize size = romFile.tellg();
    romFile.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if (!romFile.read(buffer.data(), size)) {
        std::cerr << "Load ROM file failed! " << path << "\n";
        return false;
    }

    memory.loadROM(buffer);
    std::cout << "ROM Loaded: " << path << " (" << size << " Byte(s))\n";
    return true;
}

void Emulator::run() {
    // 主循环示例
    bool running = true;
    while (running) {
        cpu.executeNextInstruction();

        // 更新其他模块
        // 处理输入
        // 渲染图形
        // 处理音频

        // 简单退出条件（可根据需要调整）
        if (cpu.shouldExit()) {
            running = false;
        }
    }
    std::cout << "Emulator exit\n";
}