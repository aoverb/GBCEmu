#pragma once
#include "Common.hpp"
#include "LCDContext.hpp"
#include "Interrupt.hpp"
#include "BusRWInterface.hpp"
#include "Bus.hpp"

namespace GBCEmu {
constexpr int LINES_PER_FRAME = 154;
constexpr int TICKS_PER_LINE = 456;
constexpr int XRES = 160;
constexpr int YRES = 144;

using Color = uint32_t;

enum class FetchState {
    TILE, DATALOW, DATAHIGH, SLEEP, PUSH
};


typedef struct PixelFIFOContext {
    FetchState state;
    std::queue<Color> pixelStack;
    uint8_t lineX;
    uint8_t pushX;
    uint8_t fetchX;
    uint8_t bgFetchData[3];
    uint8_t fetchEntryData[6];
    uint8_t mapY;
    uint8_t mapX;
    uint8_t tileY;
    uint8_t fifoX;
};

typedef struct OAM {
    uint8_t x;
    uint8_t y;
    uint8_t tile;

    unsigned cgbPaletteNum : 3;
    unsigned cgbVRAMBank : 1;
    unsigned paletteNum : 1;
    unsigned xFlip : 1;
    unsigned yFlip : 1;
    unsigned prior : 1;
};

class PPU : public BusRWInterface {
public:
    PPU(LCDContext& lcd, Bus& bus, Interrupt& interrupt);
    ~PPU();
    void tick();
    uint8_t readOAM(uint16_t addr);
    void writeOAM(uint16_t addr, uint8_t val);
    uint8_t readVRAM(uint16_t addr);
    void writeVRAM(uint16_t addr, uint8_t val);
    virtual uint8_t busRead(uint16_t addr) final;
    virtual void busWrite(uint16_t addr, uint8_t value) final;
    uint32_t getCurrentFrame();
    uint32_t* getVideoBuffer();
protected:
    OAM oam_[40];
    uint8_t vram_[0x2000];

    PixelFIFOContext pfc_;
    void pushColor(Color color);
    Color popColor();
    void pipelineFetch();
    void pipelineProcess();
    void pipelinePushPixel();
    bool pipelineAdd();

    uint32_t currentFrame_;
    uint32_t lineTicks_;
    uint32_t* videoBuffer_;
    LCDContext& lcd_;
    Bus& bus_;
    Interrupt& interrupt_;
    uint32_t prevFrameTime_;
    uint32_t startTimer_ = 0;
    uint32_t frameCount_ = 0;

    void oam();
    void xfer();
    void vBlank();
    void hBlank();
    void incLy();
};
}