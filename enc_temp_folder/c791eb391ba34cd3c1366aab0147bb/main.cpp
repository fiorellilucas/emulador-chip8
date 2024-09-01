#include <iostream>
#include <fstream>
#include <filesystem>

uint16_t memory[4096] = { 0 };
uint16_t pc = 0x200;
uint8_t regs[16] = { 0 };
uint16_t opcode;
uint16_t opcode_data;

int main() {
    using namespace std;

    filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/games/Landing.ch8");
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
        case 0x1000: {
            opcode_data = (opcode & 0x0FFF);
            pc = opcode_data;
            increment_pc = false;
            break;
        }

        case 0x2000: {
            opcode_data = (opcode & 0x0FFF);
            subroutine_call = pc;
            pc = opcode_data;
            increment_pc = false;
            break;
        }

        case 0x3000: {
            uint16_t reg_num = (opcode & 0xF00) >> 8;
            uint16_t value = (opcode & 0xFF);
            if (regs[reg_num] == value) {
                pc += 2;
            }
            break;
        }

        case 0x4000: {
            uint16_t reg_num = (opcode & 0xF00) >> 8;
            uint16_t value = (opcode & 0xFF);
            if (regs[reg_num] != value) {
                pc += 2;
            }
            break;
        }

        case 0x5000: {
            uint16_t reg_num_x = (opcode & 0xF00) >> 8;
            uint16_t reg_num_y = (opcode & 0xF0) >> 4;
            if (regs[reg_num_x] == regs[reg_num_y]) {
                pc += 2;
            }
            break;
        }

        case 0x6000: {
            uint16_t reg_num = (opcode & 0xF00) >> 8;
            uint16_t value = (opcode & 0xFF);
            regs[reg_num] = value;
            break;
        }

        case 0x7000: {
            uint16_t reg_num = (opcode & 0xF00) >> 8;
            uint16_t value = (opcode & 0xFF);
            regs[reg_num] += value;
            break;
        }

        case 0x8000: {

            uint16_t reg_num_x = (opcode & 0xF00) >> 8;
            uint16_t reg_num_y = (opcode & 0xF0) >> 4;

            switch (opcode & 0xF) {
            case 0x0: {
                regs[reg_num_x] = regs[reg_num_y];
                break;
            }
            case 0x1: {
                regs[reg_num_x] |= regs[reg_num_y];
                break;
            }
            case 0x2: {
                regs[reg_num_x] &= regs[reg_num_y];
                break;
            }
            case 0x3: {
                regs[reg_num_x] ^= regs[reg_num_y];
                break;
            }
            case 0x4: {
                regs[reg_num_x] += regs[reg_num_y];
                // VF is set to 1 when there's an overflow, and to 0 when there is not.
                break;
            }
            case 0x5: {
                regs[reg_num_x] -= regs[reg_num_y];
                // VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).
                break;
            }
            case 0x6: {
                regs[0xf] = (regs[reg_num_x] & 0x1);
                regs[reg_num_x] >>= 1;
                break;
            }
            case 0x7: {
                // VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).
                regs[reg_num_x] = regs[reg_num_y] - regs[reg_num_x];
                break;
            }
            case 0xE: {
                regs[0xf] = (regs[reg_num_x] & 0x80);
                regs[reg_num_x] <<= 1;
                break;
            }
            default: {
                cout << "opcode " << hex << showbase << opcode << " unknown\n";
                break;
            }
            }
            break;
        }

        default:
            switch (opcode) {
            case 0xEE:
                pc = subroutine_call;
                break;
            case 0xE0:
                cout << "display clear\n";
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