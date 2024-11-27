#include "PPU.hpp"

namespace GBCEmu {
    PPU::PPU(LCDContext& lcd, Bus& bus, Interrupt& interrupt) : lcd_(lcd), bus_(bus), interrupt_(interrupt)
    {
        currentFrame_ = 0;
        lineTicks_ = 0;
        videoBuffer_ = new uint32_t[XRES * YRES];
        lcd_.init();
        lcd_.setPPUMode(static_cast<uint8_t>(LCDMODE::OAM));
        prevFrameTime_ = 0;

        pfc_.lineX = 0;
        pfc_.pushX = 0;
        pfc_.fetchX = 0;
        pfc_.state = FetchState::TILE;
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
        if (addr >= 0xFE00) {
            addr -= 0xFE00;
        }
        return reinterpret_cast<uint8_t *>(oam_)[addr];
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

        return vram_[addr];
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

    uint32_t *PPU::getVideoBuffer()
    {
        return videoBuffer_;
    }

    void PPU::pushColor(Color color)
    {
        pfc_.pixelStack.push(color);
    }

    Color PPU::popColor()
    {
        Color curColor = pfc_.pixelStack.front();
        pfc_.pixelStack.pop();
        return curColor;
    }

    void PPU::pipelineFetch()
    {
        switch(pfc_.state) {
            case FetchState::TILE:
                {
                    if (lcd_.bgwEnable()) {
                        pfc_.bgFetchData[0] = bus_.read(lcd_.bgMapArea() + (pfc_.mapX / 8 + pfc_.mapY / 8 * 32) );
                        if (lcd_.bgwDataArea() == 0x8800) {
                            pfc_.bgFetchData[0] += 128;
                        }
                    }

                    pfc_.state = FetchState::DATALOW;
                    pfc_.fetchX += 8;
                    break;
                }
            case FetchState::DATALOW:
                {
                    pfc_.bgFetchData[1] = bus_.read(lcd_.bgwDataArea() + (pfc_.bgFetchData[0]) * 16 + pfc_.tileY);
                    pfc_.state = FetchState::DATAHIGH;
                    break;
                }
            case FetchState::DATAHIGH:
                {
                    pfc_.bgFetchData[2] = bus_.read(lcd_.bgwDataArea() + (pfc_.bgFetchData[0]) * 16 + pfc_.tileY + 1);
                    pfc_.state = FetchState::SLEEP;
                    break;
                }
            case FetchState::SLEEP:
                {
                    pfc_.state = FetchState::PUSH;
                    break;
                }
            case FetchState::PUSH:
                {
                    if (pipelineAdd()) {
                        pfc_.state = FetchState::TILE;
                    }
                    break;
                }
        }
    }

    void PPU::pipelineProcess()
    {
        pfc_.mapY = lcd_.ly_ + lcd_.scrY_;
        pfc_.mapX = pfc_.fetchX + lcd_.scrX_;
        pfc_.tileY = ((lcd_.ly_ + lcd_.scrY_) % 8) * 2;

        if (!(lineTicks_ & 1)) {
            pipelineFetch();
        }

        pipelinePushPixel();
    }

    void PPU::pipelinePushPixel()
    {
        if (pfc_.pixelStack.size() >= 8) {
            uint32_t pixelData = pfc_.pixelStack.front();
            pfc_.pixelStack.pop();

            if (pfc_.lineX >= lcd_.scrX_ % 8) {
                videoBuffer_[pfc_.pushX + (lcd_.ly_ * XRES)] = pixelData;
                pfc_.pushX++;
            }
            pfc_.lineX++;
        }
    }

    bool PPU::pipelineAdd()
    {
        if (pfc_.pixelStack.size() > 8) {
            return false;
        }

        int x = pfc_.fetchX - (8 - (lcd_.scrX_ % 8));
        for (int i = 0; i < 8; i++) {
            int bit = 7 - i;
            uint8_t lo = !!(pfc_.bgFetchData[1] & (1 << bit));
            uint8_t hi = !!(pfc_.bgFetchData[2] & (1 << bit)) << 1;
            Color color = lcd_.bgColor_[hi | lo];
            if (x >= 0) {
                pfc_.pixelStack.push(color);
            }
        }
        return true;
    }

    void PPU::oam()
    {
        if (lineTicks_ >= 80) {
            lcd_.setPPUMode(static_cast<uint8_t>(LCDMODE::TRANSFER));
            pfc_.state = FetchState::TILE;
            pfc_.lineX = 0;
            pfc_.pushX = 0;
            pfc_.fetchX = 0;
            pfc_.fifoX = 0;
        }
    }
    void PPU::xfer()
    {
        pipelineProcess();
        if (pfc_.pushX >= XRES) {
            while(!pfc_.pixelStack.empty()) {
                pfc_.pixelStack.pop();
            }
            lcd_.setPPUMode(static_cast<uint8_t>(LCDMODE::HBLANK));

            if (lcd_.lcds_ & static_cast<uint8_t>(STATSRC::HBLANK)) {
                interrupt_.requestInterrupt(InterruptType::LCD_STAT);
            }
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
        lcd_.ly_++;
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