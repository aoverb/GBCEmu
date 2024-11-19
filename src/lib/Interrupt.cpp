#include "Interrupt.hpp"

namespace GBCEmu {
    Interrupt::Interrupt()
    {
    }

    Interrupt::~Interrupt()
    {
    }

    void Interrupt::requestInterrupt(InterruptType interrupt)
    {
        
    }

    bool Interrupt::handleInterrupt(uint16_t& callBackAddr)
    {
        if ((intFlag_ & static_cast<uint8_t>(InterruptType::VBLANK)) && (ie_ & static_cast<uint8_t>(InterruptType::VBLANK))) {
            callBackAddr = 0x40;
            intFlag_ &= ~static_cast<uint8_t>(InterruptType::VBLANK);
            interruptEnabled_ = false;
            return false;
        } else if ((intFlag_ & static_cast<uint8_t>(InterruptType::LCD_STAT)) && (ie_ & static_cast<uint8_t>(InterruptType::LCD_STAT))) {
            callBackAddr = 0x48;
            intFlag_ &= ~static_cast<uint8_t>(InterruptType::LCD_STAT);
            interruptEnabled_ = false;
            return false;
        } else if ((intFlag_ & static_cast<uint8_t>(InterruptType::TIMER)) && (ie_ & static_cast<uint8_t>(InterruptType::TIMER))) {
            callBackAddr = 0x50;
            intFlag_ &= ~static_cast<uint8_t>(InterruptType::TIMER);
            interruptEnabled_ = false;
            return false;
        } else if ((intFlag_ & static_cast<uint8_t>(InterruptType::SERIAL)) && (ie_ & static_cast<uint8_t>(InterruptType::SERIAL))) {
            callBackAddr = 0x58;
            intFlag_ &= ~static_cast<uint8_t>(InterruptType::SERIAL);
            interruptEnabled_ = false;
            return false;
        } else if ((intFlag_ & static_cast<uint8_t>(InterruptType::JOYPAD)) && (ie_ & static_cast<uint8_t>(InterruptType::JOYPAD))) {
            callBackAddr = 0x60;
            intFlag_ &= ~static_cast<uint8_t>(InterruptType::JOYPAD);
            interruptEnabled_ = false;
            return false;
        }
        return false;
    }
}