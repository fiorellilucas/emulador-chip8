#include <iostream>
#include <fstream>
#include <filesystem>

uint16_t memory[4096] = { 0 };
uint16_t pc = 0x200;
uint16_t v[16] = { 0 };
uint16_t opcode;
uint16_t opcode_data;

int main() {
    using namespace std;

    filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/games/Pong.ch8");
    ifstream file(game_path, ios::binary);

    uint16_t game_size = filesystem::file_size(game_path);

    if (file.is_open()) {
        cout << "File opened successfully.\n";

        for (uint16_t i = 0; i < game_size; i++) {
            file.read(reinterpret_cast<char*>(&memory[0x200 + i]), 1);
            //cout << hex << showbase << memory[0x200 + i] << "\n";
        }
    }
    
    uint16_t subroutine_call = {};
    while (true) {
        opcode = (memory[pc] << 8 | memory[pc + 1]);
        bool increment_pc = true;

        switch (opcode & 0xF000) {
            case 0x1000:
                opcode_data = (opcode & 0x0FFF);
                pc = opcode_data;
                increment_pc = false;
                break;
            case 0x2000:
                opcode_data = (opcode & 0x0FFF);
                subroutine_call = pc;
                pc = opcode_data;
                increment_pc = false;     
                break;
            default:
                switch (opcode) {
                    case 0xEE:
                        pc = subroutine_call;
                        break;
                    default:
                        cout << hex << showbase << "opcode " << opcode << " unknown\n";
                        break;
                    }
        }
        if (increment_pc) {
            pc += 2;
        }
    }
    
    return 0;
}