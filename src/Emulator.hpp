// src/Emulator.h
#ifndef EMULATOR_H
#define EMULATOR_H

#include <string>
#include <vector>
#include <SDL.h>
#include "CPU.hpp"
#include "Memory.hpp"

class Emulator {
public:
    Emulator();
    ~Emulator();

    bool loadROM(const std::string& path);
    void run();

private:
    CPU cpu;
    Memory memory;
    // SDL2 相关成员
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    std::vector<uint32_t> framebuffer; // 用于存储像素数据

    // 私有方法
    bool initSDL();
    void shutdownSDL();
    void render();
};

#endif // EMULATOR_H