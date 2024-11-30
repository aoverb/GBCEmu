#include "IO.hpp"

namespace GBCEmu {
    IO::IO(Timer& timer, Interrupt& interrupt, DMA& dma, LCD& lcd, Gamepad& gamepad, APU& apu) : timer_(timer),
        interrupt_(interrupt), dma_(dma), lcd_(lcd), gamepad_(gamepad), apu_(apu)
    {
    }

    IO::~IO()
    {
    }

    uint8_t IO::read(uint16_t addr)
    {
        if (addr == 0xFF00) {
            return gamepad_.getOutput();
        }
        if (addr == 0xFF01) {
            return static_cast<uint8_t>(serialData[0]);
        }
        if (addr == 0xFF02) {
            return static_cast<uint8_t>(serialData[1]);
        }
        if (between(addr, 0xFF04, 0xFF07)) {
            return timer_.read(addr);
        }

        if (between(addr, 0xFF10, 0xFF3F)) {
            return apu_.busRead(addr);
        }

        if (addr == 0xFF0F) {
            return interrupt_.getIntFlag();
        }

        if (between(addr, 0xFF40, 0xFF4B)) {
            return lcd_.read(addr);
        }
        // std::cerr << "CPURegister::ioRead UNSUPPORTED: " << std::hex << addr << "\n";
        return 0xFF;
    }

    void IO::write(uint16_t addr, uint8_t val)
    {
        if (addr == 0xFF00) {
            gamepad_.setSel(val);
        }
        if (addr == 0xFF01) {
            serialData[0] = static_cast<char>(val);
            return;
        }
        if (addr == 0xFF02) {
            serialData[1] = static_cast<char>(val);
            return;
        }

        if (between(addr, 0xFF04, 0xFF07)) {
            timer_.write(addr, val);
            return;
        } 

        if (between(addr, 0xFF10, 0xFF3F)) {
            apu_.busWrite(addr, val);
            return;
        }

        if (addr == 0xFF0F) {
            interrupt_.setIntFlag(val);
            return;
        }

        if (between(addr, 0xFF40, 0xFF4B)) {
            lcd_.write(addr, val);
            return;
        } 
        // std::cerr << "CPURegister::ioWrite UNSUPPORTED: " << std::hex << (int)addr << std::endl;
    }
    uint8_t IO::busRead(uint16_t addr)
    {
        return read(addr);
    }
    void IO::busWrite(uint16_t addr, uint8_t value)
    {
        write(addr, value);
    }
}