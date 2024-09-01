#include <iostream>
#include <fstream>
#include <filesystem>
#include <stack>
#include <random>

using std::cout;

uint16_t memory[4096] = { 0 };
uint16_t pc = 0x200;

uint8_t regs[16] = { 0 };

std::stack<uint16_t> stack;
uint16_t sp;

uint16_t index_reg;

uint16_t opcode;
uint16_t opcode_data;

int main() {

    std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/games/Soccer.ch8");
    std::ifstream file(game_path, std::ios::binary);

    uintmax_t game_size = std::filesystem::file_size(game_path);

    if (file.is_open()) {
        cout << "File opened successfully.\n";

        for (uint16_t i = 0; i < game_size; i++) {
            file.read(reinterpret_cast<char*>(&memory[0x200 + i]), 1);
            //cout << std::hex << std::showbase << memory[0x200 + i] << "\n";
        }
    }

    while (true) {
        opcode = (memory[pc] << 8 | memory[pc + 1]);
        opcode_data = (opcode & 0xFFF);

        bool increment_pc = true;

        switch (opcode & 0xF000) {
        case 0x1000: {
            pc = opcode_data;
            increment_pc = false;
            break;
        }

        case 0x2000: {
            sp = pc;
            stack.push(sp);

            pc = opcode_data;
            increment_pc = false;
            break;
        }

        case 0x3000: {
            uint16_t reg_num = (opcode_data & 0xF00) >> 8;
            uint16_t value = (opcode_data & 0xFF);
            if (regs[reg_num] == value) {
                pc += 2;
            }
            break;
        }

        case 0x4000: {
            uint16_t reg_num = (opcode_data & 0xF00) >> 8;
            uint16_t value = (opcode_data & 0xFF);
            if (regs[reg_num] != value) {
                pc += 2;
            }
            break;
        }

        case 0x5000: {
            uint16_t reg_num_x = (opcode_data & 0xF00) >> 8;
            uint16_t reg_num_y = (opcode_data & 0xF0) >> 4;
            if (regs[reg_num_x] == regs[reg_num_y]) {
                pc += 2;
            }
            break;
        }

        case 0x6000: {
            uint16_t reg_num = (opcode_data & 0xF00) >> 8;
            uint8_t value = (opcode_data & 0xFF);
            regs[reg_num] = value;
            break;
        }

        case 0x7000: {
            uint16_t reg_num = (opcode_data & 0xF00) >> 8;
            uint8_t value = (opcode_data & 0xFF);
            regs[reg_num] += value;
            break;
        }

        case 0x8000: {
            uint16_t reg_num_x = (opcode_data & 0xF00) >> 8;
            uint16_t reg_num_y = (opcode_data & 0xF0) >> 4;

            switch (opcode_data & 0xF) {
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
                uint8_t previous_value = regs[reg_num_x];
                regs[reg_num_x] += regs[reg_num_y];
                // VF is set to 1 when there's an overflow, and to 0 when there is not.
                if (previous_value > regs[reg_num_x]) {
                    regs[0xF] = 1;
                }
                else {
                    regs[0xF] = 0;
                }
                break;
            }
            case 0x5: {
                uint8_t previous_value = regs[reg_num_x];
                regs[reg_num_x] -= regs[reg_num_y];
                // VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).
                if (previous_value < regs[reg_num_x]) {
                    regs[0xF] = 0;
                }
                else {
                    regs[0xF] = 1;
                }
                break;
            }
            case 0x6: {
                regs[0xf] = (regs[reg_num_x] & 0x1);
                regs[reg_num_x] >>= 1;
                break;
            }
            case 0x7: {
                uint8_t previous_value = regs[reg_num_x];
                regs[reg_num_x] = regs[reg_num_y] - regs[reg_num_x];
                // VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).
                if (previous_value < regs[reg_num_x]) {
                    regs[0xF] = 0;
                }
                else {
                    regs[0xF] = 1;
                }
                break;
            }
            case 0xE: {
                regs[0xf] = (regs[reg_num_x] & 0x80);
                regs[reg_num_x] <<= 1;
                break;
            }
            default: {
                cout << "opcode " << std::hex << std::showbase << opcode << " unknown\n";
                break;
            }
            }
            break;
        }

        case 0x9000: {
            uint16_t reg_num_x = (opcode_data & 0xF00) >> 8;
            uint16_t reg_num_y = (opcode_data & 0xF0) >> 4;
            if (regs[reg_num_x] != regs[reg_num_y]) {
                pc += 2;
            }
            break;
        }

        case 0xA000: {
            index_reg = opcode_data;
            break;
        }

        case 0xB000: {
            increment_pc = false;
            pc = regs[0x0] + opcode_data;
            break;
        }

        case 0xC000: {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<uint16_t> random_number(0, 255);

            uint16_t reg_num = (opcode_data & 0xF00) >> 8;
            uint8_t value = (opcode_data & 0xFF);

            regs[reg_num] = (random_number(gen) & value);
            break;
        }

        case 0xD000: {
            cout << "draw sprite\n";
            break;
        }

        default:
            switch (opcode) {
            case 0xEE:
                pc = stack.top();
                stack.pop();
                break;
            case 0xE0:
                cout << "display clear\n";
                break;
            default:
                cout << std::hex << std::showbase << "opcode " << opcode << " unknown\n";
                break;
            }
        }
        if (increment_pc) {
            pc += 2;
        }
    }

    return 0;
}