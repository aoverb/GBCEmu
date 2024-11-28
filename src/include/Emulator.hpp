// src/Emulator.h
#pragma once

#include <string>
#include <vector>
#include <SDL.h>
#include "UI.hpp"
#include "CPU.hpp"
#include "Bus.hpp"
#include "RAM.hpp"
#include "PPU.hpp"
#include "LCD.hpp"
#include "CPURegister.hpp"
#include "Cycle.hpp"
#include "DMA.hpp"
#include "Timer.hpp"
#include "Cartridge.hpp"
#include "EmuContext.hpp"
#include "Gamepad.hpp"

namespace GBCEmu {

class Emulator {
public:
    Emulator();
    ~Emulator();

    void loadROM(const std::string& path);
    int run(int argc, char* argv[]);
    void cpuRun();
    static EmuContext& getContext();
    EmuContext context_;
protected:
    CPU cpu_;
    Bus bus_;
    RAM ram_;
    UI ui_;
    IO io_;
    PPU ppu_;
    DMA dma_;
    LCD lcd_;
    Interrupt interrupt_;
    Timer timer_;
    CPURegister reg_;
    Cycle cycle_;
    Cartridge cartridge_;
    Gamepad gamepad_;
    // SDL2 相关成员
    ::SDL_Window* window_;
    ::SDL_Renderer* renderer_;
    ::SDL_Texture* texture_;
    std::vector<uint32_t> framebuffer_; // 用于存储像素数据
    
    // 私有方法
    bool initSDL();
    void shutdownSDL();
    void render();
};

}
