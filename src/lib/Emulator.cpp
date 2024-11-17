// src/Emulator.cpp
#include "Emulator.hpp"
#include <iostream>
#include <fstream>
#include <thread>

namespace GBCEmu {

// 构造函数
Emulator::Emulator() : bus_(cartridge_, ram_, reg_), cpu_(bus_, reg_, cycle_), ui_(context_)
{
    // 初始化其他模块
}

// 析构函数
Emulator::~Emulator()
{
    // 清理资源
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
    
    ui_.init();

    std::thread t([this]() {
        this->cpuRun();
    });
        
    while (!context_.die) {
        ui_.handleEvents();
        ui_.delay(10);
    }
    if (t.joinable()) {
        t.join();
    }
    std::cout << "Emulator exit\n";
}

void Emulator::cpuRun()
{
    // 主循环示例
    bool running = true;
    while (running && !context_.die) {
        if (!cpu_.step()) {
            std::cerr << "cpu failed" << "\n";
            break;
        }
        ui_.delay(100);
        // 更新其他模块
        // 处理输入
        // 渲染图形
        // 处理音频

    }

    return;
}
}