#pragma once
#include "Common.hpp"
#include "Instruction.hpp"
#include "CPURegister.hpp"

namespace GBCEmu {
    void CPURegister::setFlags(int z, int n, int h, int c)
    {
        if (z != -1) {
            setBit(f_, 7, z);
        }
        if (n != -1) {
            setBit(f_, 6, n);
        }
        if (h != -1) {
            setBit(f_, 5, h);
        }
        if (c != -1) {
            setBit(f_, 4, c);
        }
    }

    uint8_t CPURegister::ioRead(uint8_t addr)
    {
        if (addr == 0xFF01) {
            return serialData[0];
        }
        if (addr == 0xFF02) {
            return serialData[1];
        }
        // std::cerr << "CPURegister::ioRead UNSUPPORTED\n";
        return 0;
    }

    void CPURegister::ioWrite(uint8_t addr, uint8_t val)
    {
        if (addr == 0xFF01) {
            serialData[0] = val;
            return;
        }
        if (addr == 0xFF02) {
            serialData[1] = val;
            return;
        }
        // std::cerr << "CPURegister::ioWrite UNSUPPORTED\n";
    }

    uint8_t CPURegister::getZFlag()
    {
        return getBit(f_, 7);
    }
    uint8_t CPURegister::getCFlag()
    {
        return getBit(f_, 4);
    }
    uint8_t CPURegister::getHFlag()
    {
        return getBit(f_, 5);
    }
    uint8_t CPURegister::getNFlag()
    {
        return getBit(f_, 6);
    }
    uint16_t CPURegister::readReg(RegType reg)
    {
        switch(reg) {
            case RegType::A: return a_;
            case RegType::B: return b_;
            case RegType::C: return c_;
            case RegType::D: return d_;
            case RegType::E: return e_;
            case RegType::F: return f_;
            case RegType::H: return h_;
            case RegType::L: return l_;
            case RegType::BC: return (b_ << 8) | c_;
            case RegType::DE: return (d_ << 8) | e_;
            case RegType::HL: return (h_ << 8) | l_;
            case RegType::AF: return (a_ << 8) | f_;
            case RegType::SP: return sp_;
            case RegType::PC: return pc_;
            case RegType::IE: return ie_;
        }
        throw std::out_of_range("unknow writeReg type");
    }

    void CPURegister::writeReg(RegType reg, uint16_t val)
    {
        switch(reg) {
            case RegType::A: a_ = val & 0xFF; return;
            case RegType::B: b_ = val & 0xFF; return;
            case RegType::C: c_ = val & 0xFF; return;
            case RegType::D: d_ = val & 0xFF; return;
            case RegType::E: e_ = val & 0xFF; return;
            case RegType::F: f_ = val & 0xFF; return;
            case RegType::H: h_ = val & 0xFF; return;
            case RegType::L: l_ = val & 0xFF; return;
            case RegType::BC: b_ = (val & 0xFF00) >> 8; c_ = val & 0xFF; return;
            case RegType::DE: d_ = (val & 0xFF00) >> 8; e_ = val & 0xFF; return;
            case RegType::HL: h_ = (val & 0xFF00) >> 8; l_ = val & 0xFF; return;
            case RegType::AF: a_ = (val & 0xFF00) >> 8; f_ = val & 0xF0; return;
            case RegType::SP: sp_ = val; return;
            case RegType::PC: pc_ = val; return;
            case RegType::IE: ie_ = val & 0xFF; return;
        }
        throw std::out_of_range("unknow writeReg type");
    }
};