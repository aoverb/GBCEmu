#include <dbg.hpp>

namespace GBCEmu {
static char dbg_msg[1024] = {0};
static int msg_size = 0;

void dbg::dbg_update() {
    if (bus_.read(0xFF02) == 0x81) {
        char c = bus_.read(0xFF01);

        dbg_msg[msg_size++] = c;

        bus_.write(0xFF02, 0);
        dbg_print();
    }
}

void dbg::dbg_print() {
    if (dbg_msg[0]) {
        printf("DBG: %s\n", dbg_msg);
    }
}
}
