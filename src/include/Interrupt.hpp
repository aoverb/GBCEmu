#pragma once
#include "Common.hpp"

namespace GBCEmu {
enum class InterruptType {
    VBLANK = 1,
    LCD_STAT = 2,
    TIMER = 4,
    SERIAL = 8,
    JOYPAD = 16
};

class Interrupt {
public:
    Interrupt();
    ~Interrupt();
    void requestInterrupt(InterruptType interrupt);
    bool Interrupt::handleInterrupt(uint16_t& callBackAddr);
    uint8_t getIntFlags() {
        return intFlag_;
    }
    void setIntFlag(uint8_t flag) {
        intFlag_ = flag;
    }
    uint8_t getIE() {
        return ie_;
    }
    void setIE(uint8_t flag) {
        ie_ = flag;
    }
    bool getInterruptEnabled() {
        return interruptEnabled_;
    }
    void setInterruptEnabled(bool flag) {
        interruptEnabled_ = flag;
    }
    bool getEnablingIME_() {
        return enablingIME_;
    }
    void setEnablingIME_(bool flag) {
        enablingIME_ = flag;
    }
protected:
    uint8_t intFlag_ = 0;
    bool interruptEnabled_ = true;
    bool enablingIME_ = false;
    uint8_t ie_ = 0;
};
}