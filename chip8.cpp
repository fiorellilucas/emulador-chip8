#include "chip8.h"

Chip8::Chip8() {
    // load fontset
}

void Chip8::execute_opcode(uint16_t& opcode) {
    uint16_t opcode_data = (opcode & 0xFFF);

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
        if (gp_regs[reg_num] == value) {
            pc += 2;
        }
        break;
    }

    case 0x4000: {
        uint16_t reg_num = (opcode_data & 0xF00) >> 8;
        uint16_t value = (opcode_data & 0xFF);
        if (gp_regs[reg_num] != value) {
            pc += 2;
        }
        break;
    }

    case 0x5000: {
        uint16_t reg_num_x = (opcode_data & 0xF00) >> 8;
        uint16_t reg_num_y = (opcode_data & 0xF0) >> 4;
        if (gp_regs[reg_num_x] == gp_regs[reg_num_y]) {
            pc += 2;
        }
        break;
    }

    case 0x6000: {
        uint16_t reg_num = (opcode_data & 0xF00) >> 8;
        uint8_t value = (opcode_data & 0xFF);
        gp_regs[reg_num] = value;
        break;
    }

    case 0x7000: {
        uint16_t reg_num = (opcode_data & 0xF00) >> 8;
        uint8_t value = (opcode_data & 0xFF);
        gp_regs[reg_num] += value;
        break;
    }

    case 0x8000: {
        uint16_t reg_num_x = (opcode_data & 0xF00) >> 8;
        uint16_t reg_num_y = (opcode_data & 0xF0) >> 4;

        switch (opcode_data & 0xF) {
        case 0x0: {
            gp_regs[reg_num_x] = gp_regs[reg_num_y];
            break;
        }
        case 0x1: {
            gp_regs[reg_num_x] |= gp_regs[reg_num_y];
            break;
        }
        case 0x2: {
            gp_regs[reg_num_x] &= gp_regs[reg_num_y];
            break;
        }
        case 0x3: {
            gp_regs[reg_num_x] ^= gp_regs[reg_num_y];
            break;
        }
        case 0x4: {
            uint8_t previous_value = gp_regs[reg_num_x];
            gp_regs[reg_num_x] += gp_regs[reg_num_y];
            // VF is set to 1 when there's an overflow, and to 0 when there is not.
            if (previous_value > gp_regs[reg_num_x]) {
                gp_regs[0xF] = 1;
            }
            else {
                gp_regs[0xF] = 0;
            }
            break;
        }
        case 0x5: {
            uint8_t previous_value = gp_regs[reg_num_x];
            gp_regs[reg_num_x] -= gp_regs[reg_num_y];
            // VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VX >= VY and 0 if not).
            if (previous_value < gp_regs[reg_num_x]) {
                gp_regs[0xF] = 0;
            }
            else {
                gp_regs[0xF] = 1;
            }
            break;
        }
        case 0x6: {
            gp_regs[0xF] = (gp_regs[reg_num_x] & 0x1);
            gp_regs[reg_num_x] >>= 1;
            break;
        }
        case 0x7: {
            uint8_t previous_value = gp_regs[reg_num_x];
            gp_regs[reg_num_x] = gp_regs[reg_num_y] - gp_regs[reg_num_x];
            // VF is set to 0 when there's an underflow, and 1 when there is not. (i.e. VF set to 1 if VY >= VX).
            if (previous_value < gp_regs[reg_num_x]) {
                gp_regs[0xF] = 0;
            }
            else {
                gp_regs[0xF] = 1;
            }
            break;
        }
        case 0xE: {
            gp_regs[0xF] = (gp_regs[reg_num_x] & 0x80);
            gp_regs[reg_num_x] <<= 1;
            break;
        }
        default: {
            std::cout << "opcode " << std::hex << std::showbase << opcode << " unknown\n";
            break;
        }
        }
        break;
    }

    case 0x9000: {
        uint16_t reg_num_x = (opcode_data & 0xF00) >> 8;
        uint16_t reg_num_y = (opcode_data & 0xF0) >> 4;
        if (gp_regs[reg_num_x] != gp_regs[reg_num_y]) {
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
        pc = gp_regs[0x0] + opcode_data;
        break;
    }

    case 0xC000: {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint16_t> random_number(0, 255);

        uint16_t reg_num = (opcode_data & 0xF00) >> 8;
        uint8_t value = (opcode_data & 0xFF);

        gp_regs[reg_num] = (random_number(gen) & value);
        break;
    }

    case 0xD000: {
        std::cout << "draw sprite\n";
        break;
    }

    case 0xE000: {
        switch (opcode_data & 0xFF) {
        case 0x9E: {
            break;
        }
        case 0xA1: {
            break;
        }
        default: {
            std::cout << "opcode " << std::hex << std::showbase << opcode << " unknown\n";
            break;
        }
        }
        break;
    }

    case 0xF000: {
        switch (opcode_data & 0xFF) {
        case 0x07: {
            break;
        }
        case 0x0A: {
            break;
        }
        case 0x15: {
            break;
        }
        case 0x18: {
            break;
        }
        case 0x1E: {
            uint16_t reg_num = (opcode_data & 0xF00) >> 8;
            index_reg += gp_regs[reg_num];
            break;
        }
        case 0x29: {
            break;
        }
        case 0x33: {
            break;
        }
        case 0x55: {
            break;
        }
        case 0x65: {
            break;
        }
        default: {
            std::cout << "opcode " << std::hex << std::showbase << opcode << " unknown\n";
            break;
        }
        }
        break;
    }

    default:
        switch (opcode) {
        case 0xEE:
            pc = stack.top();
            stack.pop();
            break;
        case 0xE0:
            std::cout << "display clear\n";
            break;
        default:
            std::cout << std::hex << std::showbase << "opcode " << opcode << " unknown\n";
            break;
        }
    }

    if (increment_pc) {
        pc += 2;
    }
}