#include "Common.hpp"

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

    Cartridge();
    ~Cartridge();
};