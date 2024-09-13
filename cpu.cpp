#include "gpu.h"
#include "memory.h"
#include "cpu.h"

CPU::CPU(){};

void CPU::increment_pc() {
    pc += 2;
}

int CPU::decode_key_pressed() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) {
        return 0x0;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        return 0x1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        return 0x2;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
        return 0x3;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
        return 0x4;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
        return 0x5;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)) {
        return 0x6;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7)) {
        return 0x7;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8)) {
        return 0x8;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) {
        return 0x9;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        return 0xA;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
        return 0xB;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
        return 0xC;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        return 0xD;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        return 0xE;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
        return 0xF;
    }
    else {
        return NULL;
    }
}

void CPU::execute_opcode(uint16_t& opcode, Memory& mem, GPU& gpu, sf::RenderWindow& window) {
    increment_pc_flag = true;
    uint16_t opcode_data = (opcode & 0xFFF);

    uint16_t reg_num = (opcode_data & 0xF00) >> 8;
    uint16_t reg_num_x = (opcode_data & 0xF00) >> 8;
    uint16_t reg_num_y = (opcode_data & 0xF0) >> 4;

    uint16_t value = (opcode_data & 0xFF);

    switch (opcode & 0xF000) {
    case 0x1000: {
        pc = opcode_data;
        increment_pc_flag = false;
        break;
    }

    case 0x2000: {
        sp = pc;
        stack.push(sp);
        pc = opcode_data;
        increment_pc_flag = false;
        break;
    }

    case 0x3000: {
        if (gp_regs[reg_num] == value) {
            increment_pc();
        }
        break;
    }

    case 0x4000: {
        if (gp_regs[reg_num] != value) {
            increment_pc();
        }
        break;
    }

    case 0x5000: {
        if (gp_regs[reg_num_x] == gp_regs[reg_num_y]) {
            increment_pc();
        }
        break;
    }

    case 0x6000: {
        gp_regs[reg_num] = value;
        break;
    }

    case 0x7000: {
        gp_regs[reg_num] += value;
        break;
    }

    case 0x8000: {
        switch (opcode_data & 0xF) {
        case 0x0: {
            gp_regs[reg_num_x] = gp_regs[reg_num_y];
            break;
        }
        case 0x1: {
            gp_regs[reg_num_x] |= gp_regs[reg_num_y];
            gp_regs[0xf] = 0;
            break;
        }
        case 0x2: {
            gp_regs[reg_num_x] &= gp_regs[reg_num_y];
            gp_regs[0xf] = 0;
            break;
        }
        case 0x3: {
            gp_regs[reg_num_x] ^= gp_regs[reg_num_y];
            gp_regs[0xf] = 0;
            break;
        }
        case 0x4: {
            uint16_t previous_value = gp_regs[reg_num_x];
            gp_regs[reg_num_x] += gp_regs[reg_num_y];
            if (previous_value > gp_regs[reg_num_x]) {
                gp_regs[0xF] = 1;
            }
            else {
                gp_regs[0xF] = 0;
            }
            break;
        }
        case 0x5: {
            uint16_t borrow_flag = (gp_regs[reg_num_y] <= gp_regs[reg_num_x]);
            gp_regs[reg_num_x] -= gp_regs[reg_num_y];
            gp_regs[0xF] = borrow_flag;
            break;
        }
        case 0x6: {
            uint16_t carry_bit = (gp_regs[reg_num_x] & 0b1);
            gp_regs[reg_num_x] = gp_regs[reg_num_y] >> 1;
            gp_regs[0xF] = carry_bit;
            break;
        }
        case 0x7: {
            uint16_t borrow_flag = (gp_regs[reg_num_x] <= gp_regs[reg_num_y]);
            gp_regs[reg_num_x] = gp_regs[reg_num_y] - gp_regs[reg_num_x];
            gp_regs[0xF] = borrow_flag;
            break;
        }
        case 0xE: {
            uint16_t carry_bit = (gp_regs[reg_num_x] & 0b10000000) ? 1 : 0;
            gp_regs[reg_num_x] = gp_regs[reg_num_y] << 1;
            gp_regs[0xF] = carry_bit;
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
        if (gp_regs[reg_num_x] != gp_regs[reg_num_y]) {
            increment_pc();
        }
        break;
    }

    case 0xA000: {
        index_reg = opcode_data;
        break;
    }

    case 0xB000: {
        increment_pc_flag = false;
        pc = gp_regs[0x0] + opcode_data;
        break;
    }

    case 0xC000: {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint16_t> random_number(0, 255);

        gp_regs[reg_num] = (random_number(gen) & value);
        break;
    }

    case 0xD000: {
        uint16_t pos_x = gp_regs[reg_num_x] % 64;
        uint16_t pos_y = gp_regs[reg_num_y] % 32;
        uint16_t sprite_height = (opcode_data & 0xF);

        gp_regs[0xf] = 0;

        for (uint16_t row = 0; row < sprite_height; row++) {
            if ((pos_y + row) > 31) {
                break;
            }

            uint16_t sprite_row_to_be_rendered = mem.memory[index_reg + row];
            uint16_t bit_mask = 0b10000000;

            for (uint16_t pixel = 0; pixel < 8; pixel++) {
                if ((pos_x + pixel) > 63) {
                    break;
                }

                uint16_t old_pixel_state = gpu.frame_buffer[(pos_y + row) % 32][(pos_x + pixel) % 64];
                uint16_t new_pixel_state = ((sprite_row_to_be_rendered & bit_mask) ? 1 : 0) ^ old_pixel_state;

                gpu.frame_buffer[(pos_y + row) % 32][(pos_x + pixel) % 64] = new_pixel_state;

                if ((old_pixel_state == 1) && (new_pixel_state == 0)) {
                    gp_regs[0xf] = 1;
                }

                bit_mask >>= 1;
            }
        }

        break;
    }

    case 0xE000: {
        switch (opcode_data & 0xFF) {
        case 0x9E: {
            uint16_t current_key_pressed = decode_key_pressed();
            if (current_key_pressed == gp_regs[reg_num]) {
                increment_pc();
            }
            break;
        }
        case 0xA1: {
            uint16_t current_key_pressed = decode_key_pressed();
            if (current_key_pressed != gp_regs[reg_num]) {
                increment_pc();
            }
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
            gp_regs[reg_num] = delay_reg;
            break;
        }
        case 0x0A: {
            sf::Event key_event;
            while (window.waitEvent(key_event)) {
                if (key_event.type == sf::Event::KeyPressed) {
                    uint16_t current_key_pressed = decode_key_pressed();
                    gp_regs[reg_num] = current_key_pressed;
                }
                else if (key_event.type == sf::Event::KeyReleased) {
                    break;
                }
            }
            break;
        }
        case 0x15: {
            delay_reg = gp_regs[reg_num];
            break;
        }
        case 0x18: {
            sound_reg = gp_regs[reg_num];
            break;
        }
        case 0x1E: {
            index_reg += gp_regs[reg_num];
            break;
        }
        case 0x29: {
            index_reg = gp_regs[reg_num] * 0x5;
            break;
        }
        case 0x33: {
            mem.memory[index_reg] = (gp_regs[reg_num] / 100);
            mem.memory[index_reg + 1] = ((gp_regs[reg_num] % 100) / 10);
            mem.memory[index_reg + 2] = (gp_regs[reg_num] % 10);
            break;
        }
        case 0x55: {
            for (uint16_t i = 0; i <= reg_num; i++) {
                mem.memory[index_reg + i] = gp_regs[i];
            }
            index_reg += reg_num + 1;
            break;
        }
        case 0x65: {
            for (uint16_t i = 0; i <= reg_num; i++) {
                gp_regs[i] = mem.memory[index_reg + i];
            }
            index_reg += reg_num + 1;
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
            for (uint16_t pixel_pos_y = 0; pixel_pos_y < 32; pixel_pos_y++) {
                for (uint16_t pixel_pos_x = 0; pixel_pos_x < 64; pixel_pos_x++) {
                    gpu.frame_buffer[pixel_pos_y][pixel_pos_x] = 0;
                }
            }
            break;
        default:
            std::cout << std::hex << std::showbase << "opcode " << opcode << " unknown\n";
            break;
        }
    }

    if (increment_pc_flag) {
        increment_pc();
    }
}