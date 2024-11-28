#pragma once
#include "Gamepad.hpp"

namespace GBCEmu {
Gamepad::Gamepad()
{

}
Gamepad::~Gamepad()
{
}
bool Gamepad::buttonSel()
{
    return buttonSel_;
}
bool Gamepad::dirSel()
{
    return dirSel_;
}
void Gamepad::setSel(uint8_t val)
{
    buttonSel_ = val & 0x20;
    dirSel_ = val & 0x10;
}
ButtonState &Gamepad::getButtonState()
{
    return buttonState_;
}
uint8_t Gamepad::getOutput()
{
    uint8_t output = 0xCF;

    if (!buttonSel()) {
        if (buttonState_.start_) {
            output &= ~(1 << 3);
        } else if (buttonState_.select_) {
            output &= ~(1 << 2);
        } else if (buttonState_.a_) {
            output &= ~(1 << 0);
        } else if (buttonState_.b_) {
            output &= ~(1 << 1);
        }
    }

    if (!dirSel()) {
        if (buttonState_.left_) {
            output &= ~(1 << 1);
        } else if (buttonState_.right_) {
            output &= ~(1 << 0);
        } else if (buttonState_.up_) {
            output &= ~(1 << 2);
        } else if (buttonState_.down_) {
            output &= ~(1 << 3);
        }
    }
    return output;
}
uint8_t Gamepad::busRead(uint16_t addr)
{
    return 0;
}
void Gamepad::busWrite(uint16_t addr, uint8_t value)
{
    buttonSel_ = value & 0x20;
    dirSel_ = value & 0x10;
}
}