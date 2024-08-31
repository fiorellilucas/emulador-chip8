#include <iostream>
#include <fstream>
#include <filesystem>

uint16_t memory[4096] = { 0 };

int main() {
    using namespace std;

    filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/games/Pong.ch8");
    ifstream file(game_path, ios::binary);

    if (file.is_open()) {
        cout << "File opened successfully.\n";
        
        uint16_t game_size = filesystem::file_size(game_path);

        for (uint16_t i = 0; i < game_size; i++) {
            file.read(reinterpret_cast<char*>(&memory[0x200 + i]), 1);
            //cout << hex << showbase << memory[0x200 + i] << "\n";
        }

    }
    
    return 0;
}