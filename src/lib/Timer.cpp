#include "Timer.hpp"

namespace GBCEmu {
    Timer::Timer(Interrupt& interrupt) : interrupt_(interrupt)
    {
        div_ = 0xABFF;
    }

    Timer::~Timer()
    {
    }

    void Timer::tick()
    {
        uint16_t prev = div_;
        ++div_;

        bool isTimerUpdated = false;
        switch (tac_ & (0b11)) {
            case 0b00:
                isTimerUpdated = ((prev & (1 << 9)) && (!(div_ & (1 << 9))));
                break;
            case 0b01:
                isTimerUpdated = ((prev & (1 << 3)) && (!(div_ & (1 << 3))));
                break;
            case 0b10:
                isTimerUpdated = ((prev & (1 << 5)) && (!(div_ & (1 << 5))));
                break;
            case 0b11:
                isTimerUpdated = ((prev & (1 << 7)) && (!(div_ & (1 << 7))));
                break;
        }

        if (isTimerUpdated && (tac_ & (1 << 2))) {
            ++tima_;

            if (tima_ == 0xFF) {
                tima_ = tma_;
                interrupt_.requestInterrupt(InterruptType::TIMER);
            }
        }
    }
    uint8_t Timer::read(uint16_t addr)
    {
        switch (addr) {
            case 0xFF04:
                return div_ >> 8;
            case 0xFF05:
                return tima_;
            case 0xFF06:
                return tma_;
            case 0xFF07:
                return tac_;
        }

        return 0;
    }
    void Timer::write(uint16_t addr, uint8_t val)
    {
        switch (addr) {
            case 0xFF04:
                div_ = 0;
                return;
            case 0xFF05:
                tima_ = val;
                return;
            case 0xFF06:
                tma_ = val;
                return;
            case 0xFF07:
                tac_ = val;
                return;
        }
    }
}