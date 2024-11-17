// src/Emulator.cpp
#include "Emulator.hpp"
#include <iostream>
#include <fstream>

namespace GBCEmu {

// 构造函数
Emulator::Emulator() : bus_(cartridge_, ram_, reg_), cpu_(bus_, reg_, cycle_)
{
    // 初始化其他模块
}

// 析构函数
Emulator::~Emulator()
{
    // 清理资源
}

void Emulator::cycle(uint16_t cycles)
{
    // todo...
}

void Emulator::loadROM(const std::string &path)
{
    cartridge_.load(path);
}

int Emulator::run(int argc, char* argv[])
{
    std::cout << "Game Boy Color Emulator with SDL2\n";

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom path>\n";
        return -1;
    }

    std::string romPath = argv[1];

    try {
        loadROM(romPath);
    } catch (std::exception ex) {
        std::cerr << "Emulator::run catches exception: " << ex.what() << "\n";
        return -2;
    }
    

    // 主循环示例
    bool running = true;
    while (running) {
        if (!cpu_.step()) {
            std::cerr << "cpu failed" << "\n";
            break;
        }

        // 更新其他模块
        // 处理输入
        // 渲染图形
        // 处理音频

    }
    std::cout << "Emulator exit\n";
}
}