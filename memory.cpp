#include "memory.h"

Memory::Memory() {
    for (uint16_t mem_index = 0; mem_index < 80; mem_index++) {
        memory[mem_index] = fontset[mem_index];
    }
}

Memory::~Memory() {}
    std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/games/Brick (Brix hack, 1990).ch8");
    //std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/demos/Particle Demo [zeroZshadow, 2008].ch8");
    //std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/chip8-test-suite-main/bin/7-beep.ch8");
    std::ifstream file(game_path, std::ios::binary);

    if (file.is_open()) {
        uintmax_t game_size = std::filesystem::file_size(game_path);

        for (uint16_t i = 0; i < game_size; i++) {
            file.read(reinterpret_cast<char*>(&memory[PROGRAM_START_ADDRESS + i]), 1);
        }
    }
    
    return true;
}


uint16_t Memory::fetch_opcode(uint16_t& pc) const {
    return (memory[pc] << 8 | memory[pc + 1]);
}
