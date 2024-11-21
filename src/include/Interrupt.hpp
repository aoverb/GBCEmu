#pragma once
#include "Common.hpp"
#include "BusRWInterface.hpp"

namespace GBCEmu {
enum class InterruptType {
    VBLANK = 1,
    LCD_STAT = 2,
    TIMER = 4,
    SERIAL = 8,
    JOYPAD = 16
};

class Interrupt : public BusRWInterface {
public:
    Interrupt();
    ~Interrupt();
    void requestInterrupt(InterruptType interrupt);
    bool handleInterrupt(uint16_t& callBackAddr);
    uint8_t getIntFlag() {
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
    virtual uint8_t busRead(uint16_t addr) final;
    virtual void busWrite(uint16_t addr, uint8_t value) final;
protected:
    uint8_t intFlag_ = 0;
    bool interruptEnabled_ = false;
    bool enablingIME_ = false;
    uint8_t ie_ = 0;
};
}