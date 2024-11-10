// src/main.cpp
#include <iostream>
#include "Emulator.hpp"

int main(int argc, char* argv[]) {
    std::cout << "Game Boy Color Emulator with SDL2\n";

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <rom path>\n";
        return 1;
    }

    std::string romPath = argv[1];
    Emulator emulator;

    if (!emulator.loadROM(romPath)) {
        std::cerr << "Failed to load ROM: " << romPath << "\n";
        return 1;
    }

    emulator.run();

    return 0;
}