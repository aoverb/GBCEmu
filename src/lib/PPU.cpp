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
        std::cout << "readoam: " << (int)(reinterpret_cast<uint8_t *>(oam_)[addr]) << "\n";
        return reinterpret_cast<uint8_t *>(oam_)[addr];
    }
    void PPU::writeOAM(uint16_t addr, uint8_t val)
    {
        if (addr >= 0xFE00) {
            addr -= 0xFE00;
        }
        std::cout << "write oam! addr: " << std::dec << (int)addr << ", val: " << (int)val << "\n";
        
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
        } else if (addr >= 0xFE00 && addr < 0xFEA0) {
            return readOAM(addr);
        }
        throw std::out_of_range("PPU::busRead out of range!");
    }
    void PPU::busWrite(uint16_t addr, uint8_t value)
    {
        if (addr >= 0x8000 && addr < 0xA000) {
            writeVRAM(addr, value);
            return;
        } else if (addr >= 0xFE00 && addr < 0xFEA0) {
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
                    fetchedEntryCount_ = 0;
                    if (lcd_.bgwEnable()) {
                        pfc_.bgFetchData[0] = bus_.read(lcd_.bgMapArea() + (pfc_.mapX / 8 + pfc_.mapY / 8 * 32) );
                        if (lcd_.bgwDataArea() == 0x8800) {
                            pfc_.bgFetchData[0] += 128;
                        }
                    }

                    if (lcd_.objEnable() && !spriteQueue.empty()) {
                        pipelineLoadSpriteTile();
                    }

                    pfc_.state = FetchState::DATALOW;
                    pfc_.fetchX += 8;
                    break;
                }
            case FetchState::DATALOW:
                {
                    pfc_.bgFetchData[1] = bus_.read(lcd_.bgwDataArea() + (pfc_.bgFetchData[0]) * 16 + pfc_.tileY);
                    pfc_.state = FetchState::DATAHIGH;
                    pipelineLoadSpriteData(0);
                    break;
                }
            case FetchState::DATAHIGH:
                {
                    pfc_.bgFetchData[2] = bus_.read(lcd_.bgwDataArea() + (pfc_.bgFetchData[0]) * 16 + pfc_.tileY + 1);
                    pfc_.state = FetchState::SLEEP;
                    pipelineLoadSpriteData(1);
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

            if(!lcd_.bgwEnable()) {
                color = lcd_.bgColor_[0];
            }
            if(lcd_.objEnable()) {
                color = fetchSpritePixels(bit, color, hi | lo);
            }

            if (x >= 0) {
                pfc_.pixelStack.push(color);
                pfc_.fifoX++;
            }
        }
        return true;
    }

    void PPU::pipelineLoadSpriteTile()
    {
        std::priority_queue<std::pair<OAM, int>, std::vector<std::pair<OAM, int>>, SpriteComparator> localSpriteQueue = spriteQueue;
        while (!localSpriteQueue.empty()) {
            const OAM& sprite = localSpriteQueue.top().first;  // 取出精灵数据
            int index = localSpriteQueue.top().second;         // 取出精灵的索引

            int spX = (sprite.x - 8) + (lcd_.scrX_ % 8);
            if ((spX >= pfc_.fetchX && spX < pfc_.fetchX + 8) ||
                (spX + 8 >= pfc_.fetchX && spX + 8 < pfc_.fetchX + 8)) {
                fetchedEntries_[fetchedEntryCount_++] = sprite;
            }
            localSpriteQueue.pop();  // 移除当前元素

            if (localSpriteQueue.empty() || fetchedEntryCount_ >= 3) {
                break;
            }
        }
    }

    void PPU::pipelineLoadSpriteData(uint8_t offset)
    {
        int curY = lcd_.ly_;
        uint8_t spriteHeight = lcd_.objHeight();
        for (int i = 0; i < fetchedEntryCount_; i++) {
            uint8_t tileY = ((curY + 16) - fetchedEntries_[i].y) * 2;

            if (fetchedEntries_[i].yFlip) {
                tileY = ((spriteHeight * 2) - 2) - tileY;
            }

            uint8_t tileIndex = fetchedEntries_[i].tile;

            if (spriteHeight == 16) {
                tileIndex &= ~(1);
            }

            pfc_.fetchEntryData[(i * 2) + offset] = bus_.read(0x8000 + (tileIndex * 16) + tileY + offset);
        }
    }

    void PPU::loadLineSprites()
    {
        int curY = lcd_.ly_;

        uint8_t spriteHeight = lcd_.objHeight();

        for (int i = 0; i < 40; i++) {
            OAM curEntry = oam_[i];

            if (!curEntry.x) {
                continue;
            }

            if (spriteQueue.size() >= 10) {
                break;
            }
            // std::cout << "i : " << std::dec << i << "curEntry.x: " << (int)curEntry.x << "curEntry.y: " << (int)curEntry.y << "" << ", spriteHeight: " << (int)spriteHeight << ",curY + 16 = " << (int)(curY + 16) << "\n";
            if (curEntry.y <= curY + 16 && curEntry.y + spriteHeight > curY + 16) {
                spriteQueue.emplace(curEntry, i);
            }
        }
    }

    Color PPU::fetchSpritePixels(uint8_t bit, Color color, uint8_t bgColor)
    {
        for (int i = 0; i < fetchedEntryCount_; i++) {
            int spX = (fetchedEntries_[i].x - 8) + (lcd_.scrX_ % 8);

            if (spX + 8 < pfc_.fifoX) {
                continue;
            }

            int offset = pfc_.fifoX - spX;

            if (offset < 0 || offset > 7) {
                continue;
            }

            bit = (7 - offset);
            if (fetchedEntries_[i].xFlip) {
                bit = offset;
            }

            uint8_t hi = !!(pfc_.fetchEntryData[i * 2] & (1 << bit));
            uint8_t lo = !!(pfc_.fetchEntryData[i * 2 + 1] & (1 << bit)) << 1;

            bool bgPrior = fetchedEntries_[i].prior;

            if (!(hi|lo)) {
                continue;
            }

            if (!bgPrior || bgColor == 0) {
                color = fetchedEntries_[i].paletteNum ? lcd_.sp2Color_[hi|lo] : lcd_.sp1Color_[hi|lo];
                if (hi|lo) {
                    break;
                }
            }
        }

        return color;
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

        if (lineTicks_ == 1) {
            spriteQueue = std::priority_queue<std::pair<OAM, int>, std::vector<std::pair<OAM, int>>, SpriteComparator>();
            loadLineSprites();
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