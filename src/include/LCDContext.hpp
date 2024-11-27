#pragma once
#include "Common.hpp"
namespace GBCEmu {
enum class LCDMODE {
    HBLANK, VBLANK, OAM, TRANSFER
};

enum class STATSRC {
    HBLANK = (1 << 3),
    VBLANK = (1 << 4),
    OAM = (1 << 5),
    LYC = (1 << 6)
};

static uint32_t defaultColor[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};

typedef struct LCDContext {
    uint8_t lcdc_;
    uint8_t lcds_;
    uint8_t scrY_;
    uint8_t scrX_;
    uint8_t ly_;
    uint8_t lyc_;
    uint8_t dma_;
    uint8_t bgPalette_;
    uint8_t objPalette_[2];
    uint8_t winY;
    uint8_t winX;

    uint32_t bgColor_[4];
    uint32_t sp1Color_[4];
    uint32_t sp2Color_[4];

    // lcdc 0xFF40
    bool bgwEnable()
    {
        return getBit(lcdc_, 0);
    }
    bool objEnable()
    {
        return getBit(lcdc_, 1);
    }
    uint8_t objHeight()
    {
        return getBit(lcdc_, 2) ? 16 : 8;
    }
    uint16_t bgMapArea()
    {
        return getBit(lcdc_, 3) ? 0x9C00 : 0x9800;
    }
    uint16_t bgwDataArea()
    {
        return getBit(lcdc_, 4) ? 0x8000 : 0x8800;
    }
    bool winEnable()
    {
        return getBit(lcdc_, 5);
    }
    uint16_t winMapArea()
    {
        return getBit(lcdc_, 6) ? 0x9C00 : 0x9800;
    }
    bool lcdEnable()
    {
        return getBit(lcdc_, 7);
    }

    // lcds 0xFF41
    uint16_t ppuMode()
    {
        return lcds_ & 0b11;
    }

    void setPPUMode(uint8_t mode)
    {
        lcds_ &= ~0b11;
        lcds_ |= (mode & 0b11);
    }

    uint16_t lycEqLy()
    {
        return getBit(lcds_, 2);
    }

    void setLyc(uint8_t val)
    {
        return setBit(lcds_, 2, val);
    }

    bool hBlank()
    {
        return getBit(lcds_, 3);
    }

    bool vBlank()
    {
        return getBit(lcds_, 4);
    }

    bool oam()
    {
        return getBit(lcds_, 5);
    }

    bool lyc()
    {
        return getBit(lcds_, 6);
    }

    void updatePalette(uint8_t index, uint8_t data)
    {
        uint32_t* palettePtr = bgColor_;
        if (index == 1) {
            palettePtr = sp1Color_;
        } else if (index == 2) {
            palettePtr = sp2Color_;
        }

        for (int i = 0; i < 4; i++) {
            palettePtr[i] = defaultColor[(data >> (i * 2)) & 0b11];
        }
    }
};
}