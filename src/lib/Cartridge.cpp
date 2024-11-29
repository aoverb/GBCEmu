#include "Cartridge.hpp"
#include <unordered_map>
#include <fstream>
#include <sstream>

namespace GBCEmu {

const std::string ROM_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

const std::unordered_map<uint8_t, std::string> LIC_CODE = {
    {0x00, "None"},
    {0x01, "Nintendo R&D1"},
    {0x08, "Capcom"},
    {0x13, "Electronic Arts"},
    {0x18, "Hudson Soft"},
    {0x19, "b-ai"},
    {0x20, "kss"},
    {0x22, "pow"},
    {0x24, "PCM Complete"},
    {0x25, "san-x"},
    {0x28, "Kemco Japan"},
    {0x29, "seta"},
    {0x30, "Viacom"},
    {0x31, "Nintendo"},
    {0x32, "Bandai"},
    {0x33, "Ocean/Acclaim"},
    {0x34, "Konami"},
    {0x35, "Hector"},
    {0x37, "Taito"},
    {0x38, "Hudson"},
    {0x39, "Banpresto"},
    {0x41, "Ubi Soft"},
    {0x42, "Atlus"},
    {0x44, "Malibu"},
    {0x46, "angel"},
    {0x47, "Bullet-Proof"},
    {0x49, "irem"},
    {0x50, "Absolute"},
    {0x51, "Acclaim"},
    {0x52, "Activision"},
    {0x53, "American sammy"},
    {0x54, "Konami"},
    {0x55, "Hi tech entertainment"},
    {0x56, "LJN"},
    {0x57, "Matchbox"},
    {0x58, "Mattel"},
    {0x59, "Milton Bradley"},
    {0x60, "Titus"},
    {0x61, "Virgin"},
    {0x64, "LucasArts"},
    {0x67, "Ocean"},
    {0x69, "Electronic Arts"},
    {0x70, "Infogrames"},
    {0x71, "Interplay"},
    {0x72, "Broderbund"},
    {0x73, "sculptured"},
    {0x75, "sci"},
    {0x78, "THQ"},
    {0x79, "Accolade"},
    {0x80, "misawa"},
    {0x83, "lozc"},
    {0x86, "Tokuma Shoten Intermedia"},
    {0x87, "Tsukuda Original"},
    {0x91, "Chunsoft"},
    {0x92, "Video system"},
    {0x93, "Ocean/Acclaim"},
    {0x95, "Varie"},
    {0x96, "Yonezawa/s’pal"},
    {0x97, "Kaneko"},
    {0x99, "Pack in soft"},
    {0xA4, "Konami (Yu-Gi-Oh!)"}
};
void Cartridge::load(const std::string& filePath)
{
    filePath_ = filePath;
    std::cout << "load file : " << filePath << std::endl;
    std::ifstream romFile(filePath, std::ios::binary | std::ios::ate);
    if (!romFile.is_open()) {
        throw std::runtime_error("Failed to open ROM: " + filePath);
    }

    std::streamsize size = romFile.tellg();
    romFile.seekg(0, std::ios::beg);
    romData_.resize(size);
    if (!romFile.read(reinterpret_cast<char*>(romData_.data()), size)) {
        throw std::runtime_error("Load ROM file failed: " + filePath);
    }

    // 从缓冲区的 0x100 偏移处获取头部信息
    header_ = reinterpret_cast<header*>(romData_.data() + 0x100);

    // 确保标题字符串以 NULL 结尾
    header_->title[15] = '\0';
    battery_ = cartBattery();
    romNum_ = (32 << header_->rom_size) / 16;
    needSave_ = false;

    std::cout << "Cartridge Loaded:\n";
    std::cout << "\t Title    : " << header_->title << "\n";
    std::cout << "\t Type     : " << std::hex << std::uppercase << static_cast<int>(header_->type)
              << " (" << getROMType(header_->type) << ")\n";
    std::cout << "\t ROM Size : " << std::dec << (32 << header_->rom_size) << " KB\n";
    std::cout << "\t RAM Size : " << std::hex << static_cast<int>(header_->ram_size) << "\n";
    std::cout << "\t LIC Code : " << std::hex << static_cast<int>(header_->lic_code)
              << " (" << getLicCode(header_->lic_code) << ")\n";
    std::cout << "\t ROM Vers : " << std::hex << static_cast<int>(header_->version) << "\n";

    cartSetupBanking();

    uint16_t x = 0;
    // 这里需要确保访问 `romData_` 时能处理 `uint8_t` 数据类型
    for (uint16_t i = 0x0134; i <= 0x014C; i++) {
        x = x - static_cast<int>(romData_[i]) - 1;  // 转换为 int 来避免无符号运算问题
    }

    std::cout << "\t Checksum : " << std::hex << std::uppercase << static_cast<int>(header_->checksum)
         << ((x & 0xFF) ? "(PASSED)" : "(FAILED)") << "\n";

    if (battery_) {
        loadByBattery();
    }
}

// 获取授权代码对应的字符串
std::string Cartridge::getLicCode(uint8_t code)
{
    auto it = LIC_CODE.find(code);
    if(it != LIC_CODE.end()) {
        return it->second.c_str();
    }
    return "UNKNOWN";
}

std::string Cartridge::getROMType(uint8_t code)
{
    return (code > sizeof(ROM_TYPES)) ? "UNKNOWN" : ROM_TYPES[code];
}



uint8_t Cartridge::read(uint16_t addr)
{
    if ((!cartMBC1() && !cartMBC2() && !cartMBC3()) || addr < 0x4000) {
        return romData_[addr];
    }

    if (cartMBC1()) {
        return mbc1Read(addr);
    }
    if (cartMBC2()) {
        return mbc2Read(addr);
    }
    if (cartMBC3()) {
        return mbc3Read(addr);
    }
}

void Cartridge::write(uint16_t addr, uint8_t val)
{
    if (!cartMBC1() && !cartMBC2() && !cartMBC3()) {
        return; // unspported...
    }
    if (cartMBC1()) {
        mbc1Write(addr, val);
    }
    if (cartMBC2()) {
        mbc2Write(addr, val);
    }
    if (cartMBC3()) {
        mbc3Write(addr, val);
    }

}

uint8_t Cartridge::mbc1Read(uint16_t addr)
{
    if ((addr & 0xE000) == 0xA000) { // A000-C000, RAM Scope
        if (!ramEnabled_ || ramBank_ == nullptr) {
            return 0xFF;
        }

        return ramBank_[addr - 0xA000 + ramBankValue_ * 0x2000];
    }

    return romBankX_[addr - 0x4000];
}

void Cartridge::mbc1Write(uint16_t addr, uint8_t val)
{
    if (addr < 0x2000) { // RAM Enable
        ramEnabled_ = (val & 0xF) == 0xA;
        return;
    }

    if ((addr & 0xE000) == 0x2000) { // ROM bank number
        val = (val == 0) ? 1 : (val & 0b11111);
        romBankValue_ = val;
        romBankX_ = romData_.data() + (0x4000 * romBankValue_);
        // std::cout << "switching rombank to " << (int)val << "\n";
    }

    if ((addr & 0xE000) == 0x4000) { // RAM bank number
        ramBankValue_ = val & 0b11;
        if (ramBanking_) {
            if (needSave_) {
                saveByBattery();
            }
            std::cout << "switching rambank to " << (int)val << "\n";
        }
    }

    if ((addr & 0xE000) == 0x6000) { // Banking mode select
        bankingMode_ = val & 0b1;

        ramBanking_ = bankingMode_; // protection...
    }

    if ((addr & 0xE000) == 0xA000) { // RW Section
        if (!ramEnabled_ || ramBank_ == nullptr) {
            return;
        }
        std::cout << "rw..." << std::endl;
        ramBank_[addr - 0xA000 + ramBankValue_ * 0x2000] = val;

        if (battery_) {
            std::cout << "need save..." << std::endl;
            needSave_ = true;
        }
    }
}

uint8_t Cartridge::mbc2Read(uint16_t addr)
{
    if ((addr & 0xE000) == 0xA000) { // A000-C000, RAM Scope
        if (!ramEnabled_ || ramBank_ == nullptr) {
            return 0xFF;
        }

        return ramBank_[(addr - 0xA000) % 512] & 0x0F | 0xF0;
    }

    return romBankX_[addr - 0x4000];
}

void Cartridge::mbc2Write(uint16_t addr, uint8_t val)
{
    if (addr < 0x4000) {
        if (addr & 0x100) { // 8th bit, set Rom bank number...
            val = (val == 0) ? 1 : (val & 0b11111);
            romBankValue_ = val & (romNum_ - 1);
            romBankX_ = romData_.data() + (0x4000 * romBankValue_);
        } else {
            if (ramBank_ != nullptr) {
                ramEnabled_ = val == 0xA;
                return;
            }
        }
    } else if (addr >= 0xA000 && addr <= 0xBFFF) {
        if (!ramEnabled_ || ramBank_ == nullptr) {
            return;
        }
        std::cout << "rw..." << std::endl;
        ramBank_[(addr - 0xA000) % 512] = val & 0xF;
        if (battery_) {
            std::cout << "need save..." << std::endl;
            needSave_ = true;
        }
    }
}

uint8_t Cartridge::mbc3Read(uint16_t addr)
{
    return 0;
}

void Cartridge::mbc3Write(uint16_t addr, uint8_t val)
{
}

void Cartridge::loadByBattery() {
    std::stringstream ss;
    ss << filePath_ << ".save";
    std::string saveFileName = ss.str();

    std::ifstream saveFile(saveFileName, std::ios::binary);
    if (!saveFile) {
        std::cerr << "failed to open: " << saveFileName << std::endl;
        return;
    }

    // 根据ramSize_读取存档
    saveFile.read(reinterpret_cast<char*>(ramBank_), ramSize_);
    if (!saveFile) {
        std::cerr << "failed to load..." << std::endl;
    }
    saveFile.close();
}

// 保存存档
void Cartridge::saveByBattery() {
    std::stringstream ss;
    ss << filePath_ << ".save";
    std::string saveFileName = ss.str();

    std::ofstream saveFile(saveFileName, std::ios::binary);
    if (!saveFile) {
        std::cerr << "failed to open: " << saveFileName << std::endl;
        return;
    }

    saveFile.write(reinterpret_cast<const char*>(ramBank_), ramSize_);
    if (!saveFile) {
        std::cerr << "failed to save..." << std::endl;
    }
    saveFile.close();
}

void Cartridge::cartSetupBanking()
{
    int ramSize = 0;
    for (int i = 0; i < 16; i++) {
        if ((header_->ram_size == 2 && i == 0) || // one bank
            (header_->ram_size == 3 && i < 4) ||
            (header_->ram_size == 4) ||
            (header_->ram_size == 5 && i < 8)
        ) {  // four banks
            ++ramSize;
        }
    }
    ramSize_ = (cartMBC2()) ? 0x200 : ramSize * 0x2000;
    ramBank_ = new uint8_t[ramSize_];
    romBankX_ = romData_.data() + 4000;
}

uint8_t Cartridge::busRead(uint16_t addr)
{
    return read(addr);
}
void Cartridge::busWrite(uint16_t addr, uint8_t value)
{
    write(addr, value);
}
}