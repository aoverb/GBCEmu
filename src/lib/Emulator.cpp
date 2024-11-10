// src/Emulator.cpp
#include "Emulator.hpp"
#include <iostream>
#include <fstream>

namespace GBCEmu {

// 构造函数
Emulator::Emulator() : cpu_()
{
    // 初始化其他模块
}

// 析构函数
Emulator::~Emulator()
{
    // 清理资源
}


bool Emulator::loadROM(const std::string& path)
{
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

    return true;
}

int Emulator::run(int argc, char* argv[])
{
    std::cout << "Game Boy Color Emulator with SDL2\n";

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom path>\n";
        return -1;
    }

    std::string romPath = argv[1];
    Emulator emulator;

    if (!emulator.loadROM(romPath)) {
        std::cerr << "Failed to load ROM: " << romPath << "\n";
        return -2;
    }

    // 主循环示例
    bool running = true;
    while (running) {
        if (!cpu_.step()) {
            std::cerr << "cpu failed" << "\n";
        }

        // 更新其他模块
        // 处理输入
        // 渲染图形
        // 处理音频

        // 简单退出条件（可根据需要调整）
        if (cpu_.shouldExit()) {
            running = false;
        }
    }
    std::cout << "Emulator exit\n";
}
}