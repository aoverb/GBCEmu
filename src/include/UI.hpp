#pragma once
#include "EmuContext.hpp"
#include "Common.hpp"

namespace GBCEmu {
class UI {
public:
    UI(EmuContext& context);
    ~UI();
    void init();
    void delay(uint32_t ms);
    void handleEvents();
protected:
    EmuContext& context_;
};
}