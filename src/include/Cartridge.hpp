#include "Common.hpp"

namespace GBCEmu {

class Cartridge {
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
    Cartridge() {};
    ~Cartridge() {};
    void load(const std::string& filePath);
    uint8_t read(uint16_t addr);
    void write(uint16_t addr, uint8_t val);
protected:
    std::string getLicCode(uint8_t code);
    std::string getROMType(uint8_t code);
    header* header_;
    std::vector<char> romData_;
};

static_assert(sizeof(Cartridge::header) == 0x50, "Header size incorrect");

}
