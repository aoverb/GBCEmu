#include "PPU.hpp"

namespace GBCEmu {
    PPU::PPU(LCDContext& lcd, Interrupt& interrupt) : lcd_(lcd), interrupt_(interrupt)
    {
        currentFrame_ = 0;
        lineTicks_ = 0;
        videoBuffer_ = new uint32_t[XRES * YRES];
        lcd_.setPPUMode(static_cast<uint8_t>(LCDMODE::OAM));
        prevFrameTime_ = 0;
    }

    PPU::~PPU()
    {
        delete videoBuffer_;
    }

    void PPU::tick()
    {
        lineTicks_++;
        // std::cout << "tick: " << lineTicks_ << ", mode: " << lcd_.ppuMode() << ", ly: " << static_cast<int>(lcd_.ly_) <<"\n";
        switch(lcd_.ppuMode()) {
            case static_cast<uint8_t>(LCDMODE::OAM):
                oam();
                break;
            case static_cast<uint8_t>(LCDMODE::TRANSFER):
                xfer();
                break;
            case static_cast<uint8_t>(LCDMODE::VBLANK):
                vBlank();
                break;
            case static_cast<uint8_t>(LCDMODE::HBLANK):
                hBlank();
                break;
        }
    }

    uint8_t PPU::readOAM(uint16_t addr)
    {
        return vram_[addr - 0xFE00];
    }
    void PPU::writeOAM(uint16_t addr, uint8_t val)
    {
        if (addr >= 0xFE00) {
            addr -= 0xFE00;
        }

        uint8_t* ptr = reinterpret_cast<uint8_t *>(oam_);
        ptr[addr] = val;
    }
    uint8_t PPU::readVRAM(uint16_t addr)
    {
        if (addr >= 0x8000) {
            addr -= 0x8000;
        }

        uint8_t* ptr = reinterpret_cast<uint8_t *>(oam_);
        return ptr[addr];
    }
    void PPU::writeVRAM(uint16_t addr, uint8_t val)
    {
        vram_[addr - 0x8000] = val;
    }

    uint8_t PPU::busRead(uint16_t addr)
    {
        if (addr >= 0x8000 && addr < 0xA000) {
            return readVRAM(addr);
        } else if (addr >= 0xFEA0 && addr < 0xFF00) {
            return readOAM(addr);
        }
        throw std::out_of_range("PPU::busRead out of range!");
    }
    void PPU::busWrite(uint16_t addr, uint8_t value)
    {
        if (addr >= 0x8000 && addr < 0xA000) {
            writeVRAM(addr, value);
            return;
        } else if (addr >= 0xFEA0 && addr < 0xFF00) {
            writeOAM(addr, value);
            return;
        }
        throw std::out_of_range("PPU::busWrite out of range!");
    }
    uint32_t PPU::getCurrentFrame()
    {
        return currentFrame_;
    }
    void PPU::oam()
    {
        if (lineTicks_ >= 80) {
            lcd_.setPPUMode(static_cast<uint8_t>(LCDMODE::TRANSFER));
        }
    }
    void PPU::xfer()
    {
        if (lineTicks_ >= 80 + 172) {
            lcd_.setPPUMode(static_cast<uint8_t>(LCDMODE::HBLANK));
        }
    }
    void PPU::vBlank()
    {
        if (lineTicks_ >= TICKS_PER_LINE) {
            incLy();
            if (lcd_.ly_ >= LINES_PER_FRAME) {
                lcd_.setPPUMode(static_cast<uint8_t>(LCDMODE::OAM));
                lcd_.ly_ = 0;
            }

            lineTicks_ = 0;
        }        
    }
    void PPU::hBlank()
    {
        if (lineTicks_ >= TICKS_PER_LINE) {
            incLy();
            if (lcd_.ly_ >= YRES) {
                lcd_.setPPUMode(static_cast<uint8_t>(LCDMODE::VBLANK));
                interrupt_.requestInterrupt(InterruptType::VBLANK);
                if (lcd_.lcds_ & static_cast<uint8_t>(STATSRC::VBLANK)) {
                    interrupt_.requestInterrupt(InterruptType::LCD_STAT);
                }
                currentFrame_++;

                uint32_t end = getTicks();
                uint32_t frameTime = end - prevFrameTime_;

                if (frameTime < 1000 / 60) {
                    delay(1000 / 60 - frameTime);
                }

                if (end - startTimer_ >= 1000) {
                    uint32_t fps = frameCount_;
                    startTimer_ = end;
                    frameCount_ = 0;

                    printf("FPS: %ld\n", fps);
                }

                frameCount_++;
                prevFrameTime_ = getTicks();

            } else {
                lcd_.setPPUMode(static_cast<uint8_t>(LCDMODE::OAM));
            }
            lineTicks_ = 0;
        }
    }
    void PPU::incLy()
    {
        ++lcd_.ly_;
        if (lcd_.ly_ == lcd_.lyc_) {
            lcd_.setLyc(1);

            if (lcd_.lcds_ & static_cast<uint8_t>(STATSRC::LYC)) {
                interrupt_.requestInterrupt(InterruptType::LCD_STAT);
            }
        } else {
            lcd_.setLyc(0);
        }
    }
}