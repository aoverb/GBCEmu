#pragma once
#include "Common.hpp"
#include "BusRWInterface.hpp"

namespace GBCEmu {

typedef struct ButtonState
{
    bool start_ = false;
    bool select_ = false;
    bool a_ = false;
    bool b_ = false;
    bool up_ = false;
    bool down_ = false;
    bool left_ = false;
    bool right_ = false;   
};

class Gamepad : public BusRWInterface {
public:
    Gamepad();
    ~Gamepad();

    bool buttonSel();
    bool dirSel();
    void setSel(uint8_t val);
    ButtonState& getButtonState();
    uint8_t getOutput();
    uint8_t busRead(uint16_t addr) final;
    void busWrite(uint16_t addr, uint8_t value) final;
protected:
    bool buttonSel_;
    bool dirSel_;
    ButtonState buttonState_;
};

}