#include "memory.h"

Memory::Memory() {
    for (uint16_t mem_index = 0; mem_index < 80; mem_index++) {
        memory[mem_index] = fontset_[mem_index];
    }
}

Memory::~Memory() {}

bool Memory::load_game(std::filesystem::directory_entry game_rom) {
    std::ifstream file(game_rom.path(), std::ios::binary);

    if (file.is_open()) {
        uintmax_t game_size = std::filesystem::file_size(game_rom.path());

        for (uint16_t i = 0; i < game_size; i++) {
            file.read(reinterpret_cast<char*>(&memory[PROGRAM_START_ADDRESS + i]), 1);
        }
    }

    file.close();
    
    return true;
}
