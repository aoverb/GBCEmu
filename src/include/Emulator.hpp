// src/Emulator.h
#pragma once

#include <string>
#include <vector>
#include <SDL.h>
#include "CPU.hpp"
#include "Bus.hpp"
#include "Cartridge.hpp"

namespace GBCEmu {

typedef struct {
    bool paused;
    bool running;
    uint64_t ticks;
} EmuContext;

class Emulator {
public:
    Emulator();
    ~Emulator();

    void cycle(uint16_t cycles);
    void loadROM(const std::string& path);
    int run(int argc, char* argv[]);
    static EmuContext& getContext();
    static EmuContext context_;
protected:
    CPU cpu_;
    Bus bus_;
    Cartridge cartridge_;
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
