#include "UI.hpp"
#include "sstream"
namespace GBCEmu {
    UI::UI(EmuContext& context, Bus& bus, PPU& ppu, Gamepad& gamepad) : context_(context), bus_(bus), ppu_(ppu), gamepad_(gamepad)
    {
    }

    UI::~UI()
    {
    }

    void UI::init() {
        SDL_Init(SDL_INIT_VIDEO);
        printf("SDL INIT\n");

        SDL_CreateWindowAndRenderer(screenWidth_, screenHeight_, 0, &sdlWindow_, &sdlRenderer_);


        SDL_CreateWindowAndRenderer(16 * 8 * scale_, 32 * 8 * scale_, 0, &sdlDbgWindow_, &sdlDbgRenderer_);

        screen_ = SDL_CreateRGBSurface(0, screenWidth_, screenHeight_, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

        dbgScreen_ = SDL_CreateRGBSurface(0, 16 * 8 * scale_ + 16 * scale_, 32 * 8 * scale_ + 64 * scale_, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        sdlTexture_ = SDL_CreateTexture(sdlRenderer_, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
            screenWidth_, screenHeight_);
        sdlDbgTexture_ = SDL_CreateTexture(sdlDbgRenderer_, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING,
            16 * 8 * scale_ + 16 * scale_, 32 * 8 * scale_ + 64 * scale_);
        int x, y;
        SDL_GetWindowPosition(sdlWindow_, &x, &y);
        SDL_SetWindowPosition(sdlDbgWindow_, x + screenWidth_ + 10, y);
    }

    void UI::delay(uint32_t ms) {
        SDL_Delay(ms);
    }

    void UI::onKey(bool down, uint32_t keyCode) {
        switch (keyCode) {
            case SDLK_z:
                gamepad_.getButtonState().b_ = down; break;
            case SDLK_x:
                gamepad_.getButtonState().a_ = down; break;
            case SDLK_s:
                gamepad_.getButtonState().start_ = down; break;
            case SDLK_a:
                gamepad_.getButtonState().select_ = down; break;
            case SDLK_UP:
                gamepad_.getButtonState().up_ = down; break;
            case SDLK_DOWN:
                gamepad_.getButtonState().down_ = down; break;
            case SDLK_LEFT:
                gamepad_.getButtonState().left_ = down; break;
            case SDLK_RIGHT:
                gamepad_.getButtonState().right_ = down; break;
        }
    }

    void UI::handleEvents() {
        SDL_Event e;
        if (SDL_PollEvent(&e) > 0)
        {
            if (e.type == SDL_KEYDOWN) {
                onKey(true, e.key.keysym.sym);
            }
            if (e.type == SDL_KEYUP) {
                onKey(false, e.key.keysym.sym);
            }
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
                context_.die = true;
            }
        }
    }
    void UI::update()
    {
        updateEmuWindow();
        updateDebugWindow();
    }

    void UI::updateEmuWindow()
    {
        std::stringstream ss;
        ss << "FPS: " << context_.fps;
        SDL_SetWindowTitle(sdlWindow_, ss.str().c_str());
        SDL_Rect rc;
        rc.x = rc.y = 0;
        rc.w = 2048;
        rc.h = 2048;
        Color* videoBuffer = ppu_.getVideoBuffer();
        for (int lineNum = 0; lineNum < YRES; ++lineNum) {
            for (int x = 0; x < XRES; ++x) {
                rc.x = x * scale_;
                rc.y = lineNum * scale_;
                rc.w = scale_;
                rc.h = scale_;
                SDL_FillRect(screen_, &rc, videoBuffer[x + (lineNum * XRES)]);
            }
        }
        SDL_UpdateTexture(sdlTexture_, NULL, screen_->pixels,screen_->pitch);
        SDL_RenderClear(sdlRenderer_);
        SDL_RenderCopy(sdlRenderer_, sdlTexture_, NULL, NULL);
        SDL_RenderPresent(sdlRenderer_);
    }

    void UI::updateDebugWindow()
    {
        int xDraw = 0;
        int yDraw = 0;
        int tileNum = 0;
        SDL_Rect rc;
        rc.x = 0;
        rc.y = 0;
        rc.w = dbgScreen_->w;
        rc.h = dbgScreen_->h;

        // SDL_FillRect(dbgScreen_, &rc, 0xFF11111111);

        uint16_t addr = 0x8000;

        for (int y = 0; y < 24; y++) {
            for (int x = 0; x < 16; x++) {
                displayTile(dbgScreen_, addr, tileNum++, xDraw + (x * scale_), yDraw + (y * scale_));
                xDraw += (8 * scale_);
            }
            yDraw += (8 * scale_);
            xDraw = 0;
        }

        SDL_UpdateTexture(sdlDbgTexture_, NULL, dbgScreen_->pixels, dbgScreen_->pitch);
        SDL_RenderClear(sdlDbgRenderer_);
        SDL_RenderCopy(sdlDbgRenderer_, sdlDbgTexture_, NULL, NULL);
        SDL_RenderPresent(sdlDbgRenderer_);
    }

    void UI::displayTile(SDL_Surface *surface, uint16_t startLoc, uint16_t tileNum, int x, int y)
    {
        SDL_Rect rc;
        for (int tileY = 0; tileY < 16; tileY += 2) {
            uint8_t b2 = bus_.read(startLoc + (tileNum * 16) + tileY);
            uint8_t b1 = bus_.read(startLoc + (tileNum * 16) + tileY + 1);

            for (int bit = 7; bit >= 0; bit--) {
                uint8_t hi = !!(b1 & (1 << bit)) << 1;
                uint8_t lo = !!(b2 & (1 << bit));
                uint8_t color = hi | lo;
                rc.x = x + ((7 - bit) * scale_);
                rc.y = y + (tileY / 2 * scale_);
                rc.w = scale_;
                rc.h = scale_;

                SDL_FillRect(surface, &rc, color_[color]);
            }
        }
    }
}