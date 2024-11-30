#pragma once
#include <bus.hpp>

namespace GBCEmu {
class dbg {
public:
    dbg(Bus& bus) : bus_(bus) {};
    void dbg_update();
    void dbg_print();
protected:
    Bus& bus_;
};

}