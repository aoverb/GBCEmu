#include "Common.hpp"
#include <SDL.h>
namespace GBCEmu {
void delay(uint32_t ms)
{
    ::SDL_Delay(ms);
}

uint32_t getTicks()
{
    return ::SDL_GetTicks();
}

}