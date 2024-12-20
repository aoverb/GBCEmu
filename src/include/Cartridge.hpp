#pragma once
#include "Common.hpp"
#include "RTC.hpp"
#include "BusRWInterface.hpp"
namespace GBCEmu {

class Cartridge : public BusRWInterface {
public:
    typedef struct {
        uint8_t entry[4];
        uint8_t logo[0x30];
        char title[16];
        uint16_t new_lic_code;
        uint8_t sgb_flag;
        uint8_t type;
        uint8_t rom_size;
        uint8_t ram_size;
        uint8_t dest_code;
        uint8_t lic_code;
        uint8_t version;
        uint8_t checksum;
        uint16_t global_checksum;
    } header;
    Cartridge(RTC& rtc);
    ~Cartridge() {};
    void load(const std::string& filePath);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
    uint8_t mbc1Read(uint16_t addr);
    void mbc1Write(uint16_t addr, uint8_t val);
    uint8_t mbc2Read(uint16_t addr);
    void mbc2Write(uint16_t addr, uint8_t val);
    uint8_t mbc3Read(uint16_t addr);
    void mbc3Write(uint16_t addr, uint8_t val);
    virtual uint8_t busRead(uint16_t addr) final;
    virtual void busWrite(uint16_t addr, uint8_t value) final;
    void loadByBattery();
    void saveByBattery();
    bool needSave()
    {
        return needSave_;
    }
    bool needTimer()
    {
        return between(header_->type, 15, 16);
    }
protected:
    std::string getLicCode(uint8_t code);
    std::string getROMType(uint8_t code);
    bool cartMBC1()
    {
        return between(header_->type, 1, 3);
    }
    bool cartMBC2()
    {
        return between(header_->type, 5, 6);
    }
    bool cartMBC3()
    {
        return between(header_->type, 15, 19);
    }
    bool cartBattery()
    {
        return header_->type != 0 && ((header_->type < 10 && header_->type % 3 == 0) || header_->type == 13
            || header_->type == 15 || header_->type == 16 || header_->type == 19);
    }
    void cartSetupBanking();
    header* header_;
    std::vector<uint8_t> romData_;

    bool ramEnabled_ = false;
    bool ramBanking_;
    uint32_t ramSize_;

    uint8_t* romBankX_;
    uint8_t bankingMode_;

    uint8_t romNum_ = 0;
    uint8_t romBankValue_ = 1;
    uint8_t ramBankValue_ = 0;

    uint8_t *ramBank_;

    bool battery_;
    bool needSave_;
    std::string filePath_;

    RTC& rtc_;
};

static_assert(sizeof(Cartridge::header) == 0x50, "Header size incorrect");

}
