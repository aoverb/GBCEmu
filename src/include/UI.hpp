#pragma once
#include "EmuContext.hpp"
#include "Common.hpp"
#include "Bus.hpp"
#include "PPU.hpp"
#include <SDL.h>

namespace GBCEmu {
class UI {
public:
    UI(EmuContext& context, Bus& bus, PPU& ppu);
    ~UI();
    void init();
    void delay(uint32_t ms);
    void handleEvents();
    void update();
protected:
    Bus& bus_;
    PPU& ppu_;
    void updateDebugWindow();
    void updateEmuWindow();
    void displayTile(SDL_Surface* surface, uint16_t startLoc, uint16_t tileNum, int x, int y);
    void displaySprite(SDL_Surface *surface, uint16_t startLoc, uint16_t spriteNum, int x, int y);
    uint32_t color_[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
    uint16_t screenWidth_ = 660;
    uint16_t screenHeight_ = 600;
    uint8_t scale_ = 4;
    EmuContext& context_;
    SDL_Window *sdlWindow_;
    SDL_Renderer *sdlRenderer_;
    SDL_Texture *sdlTexture_;
    SDL_Surface *screen_;

    SDL_Window *sdlDbgWindow_;
    SDL_Renderer *sdlDbgRenderer_;
    SDL_Texture *sdlDbgTexture_;
    SDL_Surface *dbgScreen_;
};
}