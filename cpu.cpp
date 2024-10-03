#include "gpu.h"
#include "memory.h"
#include "cpu.h"

CPU::CPU() {}

CPU::~CPU() {}

uint16_t CPU::get_delay() {
    return delay_reg_;
}

uint16_t CPU::get_sound() {
    return sound_reg_;
}

void CPU::decrement_delay() {
    delay_reg_--;
}

void CPU::decrement_sound() {
    sound_reg_--;
}

void CPU::set_opcode_(uint16_t opcode) {
    opcode_ = opcode;
}

void CPU::increment_pc_() {
    pc += 2;
}

void CPU::fetch_opcode(Memory& mem) {
    set_opcode_(mem.memory[pc] << 8 | mem.memory[pc + 1]);
}

void CPU::execute_opcode(Memory& mem, GPU& gpu, sf::RenderWindow& window, uint16_t& key_pressed) {
    increment_pc_flag_ = true;
    uint16_t opcode_data = (opcode_ & 0xFFF);

    uint16_t reg_num = (opcode_data & 0xF00) >> 8;
    uint16_t reg_num_x = (opcode_data & 0xF00) >> 8;
    uint16_t reg_num_y = (opcode_data & 0xF0) >> 4;

    uint16_t value = (opcode_data & 0xFF);

    switch (opcode_ & 0xF000) {
    case 0x1000: {
        pc = opcode_data;
        increment_pc_flag_ = false;
        break;
    }

    case 0x2000: {
        sp_ = pc;
        stack_.push(sp_);
        pc = opcode_data;
        increment_pc_flag_ = false;
        break;
    }

    case 0x3000: {
        if (gp_regs_[reg_num] == value) {
            increment_pc_();
        }
        break;
    }

    case 0x4000: {
        if (gp_regs_[reg_num] != value) {
            increment_pc_();
        }
        break;
    }

    case 0x5000: {
        if (gp_regs_[reg_num_x] == gp_regs_[reg_num_y]) {
            increment_pc_();
        }
        break;
    }

    case 0x6000: {
        gp_regs_[reg_num] = value;
        break;
    }

    case 0x7000: {
        gp_regs_[reg_num] += value;
        break;
    }

    case 0x8000: {
        switch (opcode_data & 0xF) {
        case 0x0: {
            gp_regs_[reg_num_x] = gp_regs_[reg_num_y];
            break;
        }
        case 0x1: {
            gp_regs_[reg_num_x] |= gp_regs_[reg_num_y];
            gp_regs_[0xf] = 0;
            break;
        }
        case 0x2: {
            gp_regs_[reg_num_x] &= gp_regs_[reg_num_y];
            gp_regs_[0xf] = 0;
            break;
        }
        case 0x3: {
            gp_regs_[reg_num_x] ^= gp_regs_[reg_num_y];
            gp_regs_[0xf] = 0;
            break;
        }
        case 0x4: {
            uint16_t previous_value = gp_regs_[reg_num_x];
            gp_regs_[reg_num_x] += gp_regs_[reg_num_y];
            if (previous_value > gp_regs_[reg_num_x]) {
                gp_regs_[0xF] = 1;
            }
            else {
                gp_regs_[0xF] = 0;
            }
            break;
        }
        case 0x5: {
            uint16_t borrow_flag = (gp_regs_[reg_num_y] <= gp_regs_[reg_num_x]);
            gp_regs_[reg_num_x] -= gp_regs_[reg_num_y];
            gp_regs_[0xF] = borrow_flag;
            break;
        }
        case 0x6: {
            uint16_t carry_bit = (gp_regs_[reg_num_x] & 0b1);
            gp_regs_[reg_num_x] = gp_regs_[reg_num_y] >> 1;
            gp_regs_[0xF] = carry_bit;
            break;
        }
        case 0x7: {
            uint16_t borrow_flag = (gp_regs_[reg_num_x] <= gp_regs_[reg_num_y]);
            gp_regs_[reg_num_x] = gp_regs_[reg_num_y] - gp_regs_[reg_num_x];
            gp_regs_[0xF] = borrow_flag;
            break;
        }
        case 0xE: {
            uint16_t carry_bit = (gp_regs_[reg_num_x] & 0b10000000) ? 1 : 0;
            gp_regs_[reg_num_x] = gp_regs_[reg_num_y] << 1;
            gp_regs_[0xF] = carry_bit;
            break;
        }
        default: {
            std::cout << "opcode " << std::hex << std::showbase << opcode_ << " unknown\n";
            break;
        }
        }
        break;
    }

    case 0x9000: {
        if (gp_regs_[reg_num_x] != gp_regs_[reg_num_y]) {
            increment_pc_();
        }
        break;
    }

    case 0xA000: {
        index_reg_ = opcode_data;
        break;
    }

    case 0xB000: {
        increment_pc_flag_ = false;
        pc = gp_regs_[0x0] + opcode_data;
        break;
    }

    case 0xC000: {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint16_t> random_number(0, 255);

        gp_regs_[reg_num] = (random_number(gen) & value);
        break;
    }

    case 0xD000: {
        uint16_t pos_x = gp_regs_[reg_num_x] % 64;
        uint16_t pos_y = gp_regs_[reg_num_y] % 32;
        uint16_t sprite_height = (opcode_data & 0xF);

        gp_regs_[0xf] = 0;

        for (uint16_t row = 0; row < sprite_height; row++) {
            if ((pos_y + row) > 31) {
                break;
            }

            uint16_t sprite_row_to_be_rendered = mem.memory[index_reg_ + row];
            uint16_t bit_mask = 0b10000000;

            for (uint16_t pixel = 0; pixel < 8; pixel++) {
                if ((pos_x + pixel) > 63) {
                    break;
                }

                uint16_t old_pixel_state = gpu.frame_buffer[(pos_y + row) % 32][(pos_x + pixel) % 64];
                uint16_t new_pixel_state = ((sprite_row_to_be_rendered & bit_mask) ? 1 : 0) ^ old_pixel_state;

                gpu.frame_buffer[(pos_y + row) % 32][(pos_x + pixel) % 64] = new_pixel_state;

                if ((old_pixel_state == 1) && (new_pixel_state == 0)) {
                    gp_regs_[0xf] = 1;
                }

                bit_mask >>= 1;
            }
        }

        break;
    }

    case 0xE000: {
        switch (opcode_data & 0xFF) {
        case 0x9E: {
            if (key_pressed == gp_regs_[reg_num]) {
                increment_pc_();
            }
            break;
        }
        case 0xA1: {
            if (key_pressed != gp_regs_[reg_num]) {
                increment_pc_();
            }
            break;
        }

        default: {
            std::cout << "opcode " << std::hex << std::showbase << opcode_ << " unknown\n";
            break;
        }
        }
        break;
    }

    case 0xF000: {
        switch (opcode_data & 0xFF) {
        case 0x07: {
            gp_regs_[reg_num] = delay_reg_;
            break;
        }
        case 0x0A: {
            sf::Event event;
            while (window.waitEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed) {
                    gp_regs_[reg_num] = key_pressed;
                }
                else if (event.type == sf::Event::KeyReleased) {
                    break;
                }
            }
            break;
        }
        case 0x15: {
            delay_reg_ = gp_regs_[reg_num];
            break;
        }
        case 0x18: {
            sound_reg_ = gp_regs_[reg_num];
            break;
        }
        case 0x1E: {
            index_reg_ += gp_regs_[reg_num];
            break;
        }
        case 0x29: {
            index_reg_ = gp_regs_[reg_num] * 0x5;
            break;
        }
        case 0x33: {
            mem.memory[index_reg_] = (gp_regs_[reg_num] / 100);
            mem.memory[index_reg_ + 1] = ((gp_regs_[reg_num] % 100) / 10);
            mem.memory[index_reg_ + 2] = (gp_regs_[reg_num] % 10);
            break;
        }
        case 0x55: {
            for (uint16_t i = 0; i <= reg_num; i++) {
                mem.memory[index_reg_ + i] = gp_regs_[i];
            }
            index_reg_ += reg_num + 1;
            break;
        }
        case 0x65: {
            for (uint16_t i = 0; i <= reg_num; i++) {
                gp_regs_[i] = mem.memory[index_reg_ + i];
            }
            index_reg_ += reg_num + 1;
            break;
        }
        default: {
            std::cout << "opcode " << std::hex << std::showbase << opcode_ << " unknown\n";
            break;
        }
        }
        break;
    }

    default:
        switch (opcode_) {
        case 0xEE:
            pc = stack_.top();
            stack_.pop();
            break;
        case 0xE0:
            for (uint16_t pixel_pos_y = 0; pixel_pos_y < 32; pixel_pos_y++) {
                for (uint16_t pixel_pos_x = 0; pixel_pos_x < 64; pixel_pos_x++) {
                    gpu.frame_buffer[pixel_pos_y][pixel_pos_x] = 0;
                }
            }
            break;
        default:
            std::cout << std::hex << std::showbase << "opcode " << opcode_ << " unknown\n";
            break;
        }
    }

    if (increment_pc_flag_) {
        increment_pc_();
    }
}