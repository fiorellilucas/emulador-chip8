#include "main.h"
#include "chip8.h"

void load_game(Chip8& chip8);

int main() {
    Chip8 chip8;
    load_game(chip8);

    sf::RenderWindow window(sf::VideoMode(1280, 640), "SFML works!");
    
    //window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    window.clear();
    window.display();
    
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        uint16_t opcode = chip8.fetch_opcode();
        std::cout << std::hex << std::showbase << opcode << "\n";
        window.clear();
        chip8.execute_opcode(opcode, window);
    }
    return 0;
}

void load_game(Chip8& chip8) {
    std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/chip8-test-suite-main/bin/7-beep.ch8");
    std::ifstream file(game_path, std::ios::binary);

    if (file.is_open()) {
        uintmax_t game_size = std::filesystem::file_size(game_path);

        for (uint16_t i = 0; i < game_size; i++) {
            file.read(reinterpret_cast<char*>(&chip8.memory[chip8.pc + i]), 1);
        }
    }
}