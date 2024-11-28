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

constexpr int OAM_LENGTH = 40;

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
#pragma pack(push, 1)
typedef struct OAM {
    uint8_t y;
    uint8_t x;
    uint8_t tile;

    uint8_t cgbPaletteNum : 3;
    uint8_t cgbVRAMBank : 1;
    uint8_t paletteNum : 1;
    uint8_t xFlip : 1;
    uint8_t yFlip : 1;
    uint8_t prior : 1;
};
#pragma pack(pop)

struct SpriteComparator {
    bool operator()(const std::pair<OAM, int>& a, const std::pair<OAM, int>& b) const {
        // 先按X坐标排序，X坐标小的优先
        if (a.first.x != b.first.x) {
            return a.first.x > b.first.x;  // X坐标小优先
        }
        // 如果X坐标相同，按OAM索引排序
        return a.second > b.second;  // OAM索引小优先
    }
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
    OAM oam_[OAM_LENGTH];
    uint8_t vram_[0x2000];

    PixelFIFOContext pfc_;
    void pushColor(Color color);
    Color popColor();
    void pipelineLoadWindowTile();
    void pipelineFetch();
    void pipelineProcess();
    void pipelinePushPixel();
    bool pipelineAdd();
    void pipelineLoadSpriteTile();
    void pipelineLoadSpriteData(uint8_t offset);

    bool windowVisible();

    void loadLineSprites();
    Color fetchSpritePixels(uint8_t bit, Color color, uint8_t bgColor);

    uint32_t currentFrame_;
    uint32_t lineTicks_;
    uint32_t* videoBuffer_;
    LCDContext& lcd_;
    Bus& bus_;
    Interrupt& interrupt_;
    uint32_t prevFrameTime_;
    uint32_t startTimer_ = 0;
    uint32_t frameCount_ = 0;

    std::priority_queue<std::pair<OAM, int>, std::vector<std::pair<OAM, int>>, SpriteComparator> spriteQueue;

    uint8_t fetchedEntryCount_ = 0;
    OAM fetchedEntries_[3];

    uint8_t windowLine_ = 0;

    void oam();
    void xfer();
    void vBlank();
    void hBlank();
    void incLy();
};
}