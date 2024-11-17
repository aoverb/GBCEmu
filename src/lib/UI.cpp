#include "UI.hpp"
#include <SDL.h>

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Texture *sdlTexture;
SDL_Surface *screen;

namespace GBCEmu {
    UI::UI(EmuContext& context) : context_(context)
    {
    }

    UI::~UI()
    {
    }

    void UI::init() {
        SDL_Init(SDL_INIT_VIDEO);
        printf("SDL INIT\n");

        SDL_CreateWindowAndRenderer(768, 1024, 0, &sdlWindow, &sdlRenderer);
    }

    void UI::delay(uint32_t ms) {
        SDL_Delay(ms);
    }

    void UI::handleEvents() {
        SDL_Event e;
        if (SDL_PollEvent(&e) > 0)
        {   
            //TODO SDL_UpdateWindowSurface(sdlWindow);
            //TODO SDL_UpdateWindowSurface(sdlTraceWindow);
            //TODO SDL_UpdateWindowSurface(sdlDebugWindow);
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
                context_.die = true;
            }
        }
    }
}