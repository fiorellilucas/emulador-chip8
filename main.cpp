#include "main.h"
#include "chip8.h"

int main() {
    std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/games/Animal Race [Brian Astle].ch8");
    std::ifstream file(game_path, std::ios::binary);

    Chip8 chip8;

    if (file.is_open()) {
        std::cout << "File opened successfully.\n";
        uintmax_t game_size = std::filesystem::file_size(game_path);

        for (uint16_t i = 0; i < game_size; i++) {
            file.read(reinterpret_cast<char*>(&chip8.memory[chip8.pc + i]), 1);
            //cout << std::hex << std::showbase << memory[0x200 + i] << "\n";
        }
    }

    while (true) {
        uint16_t opcode = (chip8.memory[chip8.pc] << 8 | chip8.memory[chip8.pc + 1]);
        std::cout << std::hex << std::showbase << opcode << "\n";
        
        chip8.execute_opcode(opcode);
    }

    return 0;
}