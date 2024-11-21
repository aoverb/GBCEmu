// src/Emulator.cpp
#include "Emulator.hpp"
#include <iostream>
#include <fstream>
#include <thread>

namespace GBCEmu {

// 构造函数
Emulator::Emulator() : ui_(context_, bus_), timer_(interrupt_), cycle_(context_, timer_), io_(timer_, interrupt_),
    cpu_(bus_, reg_, cycle_, interrupt_)
{
    bus_.regDevice(0x0000, 0x8000 - 0x1, cartridge_);
    bus_.regDevice(0x8000, 0xA000 - 0x1, ppu_);
    bus_.regDevice(0xA000, 0xC000 - 0x1, cartridge_);
    bus_.regDevice(0xC000, 0xE000 - 0x1, ram_);
    bus_.regDevice(0xFEA0, 0xFF00 - 0x1, ppu_);
    bus_.regDevice(0xFF00, 0xFF80 - 0x1, io_);
    bus_.regDevice(0xFF80, 0xFFFF - 0x1, ram_);
    bus_.regDevice(0xFFFF, 0xFFFF, interrupt_);
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
    std::cout << "cpuRun...\n";
    std::thread t([this]() {
        this->cpuRun();
    });
        
    while (!context_.die) {
        ui_.handleEvents();
        ui_.delay(10);
        ui_.update();
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

        // 更新其他模块
        // 处理输入
        // 渲染图形
        // 处理音频

    }

    return;
}
}