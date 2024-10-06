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

void CPU::op_00E0(GPU& gpu) {
    for (uint16_t pixel_pos_y = 0; pixel_pos_y < 32; pixel_pos_y++) {
        for (uint16_t pixel_pos_x = 0; pixel_pos_x < 64; pixel_pos_x++) {
            gpu.frame_buffer[pixel_pos_y][pixel_pos_x] = 0;
        }
    }
}

void CPU::op_00EE() {
    pc = stack_.top();
    stack_.pop();
}

void CPU::op_1nnn(uint16_t opcode_data) {
    pc = opcode_data;
    increment_pc_flag_ = false;
}

void CPU::op_2nnn(uint16_t opcode_data) {
    sp_ = pc;
    stack_.push(sp_);
    pc = opcode_data;
    increment_pc_flag_ = false;
}

void CPU::op_3xnn(uint16_t reg_num, uint16_t value) {
    if (gp_regs_[reg_num] == value) {
        increment_pc_();
    }
}

void CPU::op_4xnn(uint16_t reg_num, uint16_t value) {
    if (gp_regs_[reg_num] != value) {
        increment_pc_();
    }
}

void CPU::op_5xy0(uint16_t reg_num_x, uint16_t reg_num_y) {
    if (gp_regs_[reg_num_x] == gp_regs_[reg_num_y]) {
        increment_pc_();
    }
}

void CPU::op_6xnn(uint16_t reg_num, uint16_t value) {
    gp_regs_[reg_num] = value;
}

void CPU::op_7xnn(uint16_t reg_num, uint16_t value) {
    gp_regs_[reg_num] += value;
}

void CPU::op_8xy0(uint16_t reg_num_x, uint16_t reg_num_y) {
    gp_regs_[reg_num_x] = gp_regs_[reg_num_y];
}

void CPU::op_8xy1(uint16_t reg_num_x, uint16_t reg_num_y) {
    gp_regs_[reg_num_x] |= gp_regs_[reg_num_y];
    gp_regs_[0xf] = 0;
}

void CPU::op_8xy2(uint16_t reg_num_x, uint16_t reg_num_y) {
    gp_regs_[reg_num_x] &= gp_regs_[reg_num_y];
    gp_regs_[0xf] = 0;
}

void CPU::op_8xy3(uint16_t reg_num_x, uint16_t reg_num_y) {
    gp_regs_[reg_num_x] ^= gp_regs_[reg_num_y];
    gp_regs_[0xf] = 0;
}

void CPU::op_8xy4(uint16_t reg_num_x, uint16_t reg_num_y) {
    uint16_t previous_value = gp_regs_[reg_num_x];
    gp_regs_[reg_num_x] += gp_regs_[reg_num_y];
    if (previous_value > gp_regs_[reg_num_x]) {
        gp_regs_[0xF] = 1;
    }
    else {
        gp_regs_[0xF] = 0;
    }
}

void CPU::op_8xy5(uint16_t reg_num_x, uint16_t reg_num_y) {
    uint16_t borrow_flag = (gp_regs_[reg_num_y] <= gp_regs_[reg_num_x]);
    gp_regs_[reg_num_x] -= gp_regs_[reg_num_y];
    gp_regs_[0xF] = borrow_flag;
}

void CPU::op_8xy6(uint16_t reg_num_x, uint16_t reg_num_y) {
    uint16_t carry_bit = (gp_regs_[reg_num_x] & 0b1);
    gp_regs_[reg_num_x] = gp_regs_[reg_num_y] >> 1;
    gp_regs_[0xF] = carry_bit;
}

void CPU::op_8xy7(uint16_t reg_num_x, uint16_t reg_num_y) {
    uint16_t borrow_flag = (gp_regs_[reg_num_x] <= gp_regs_[reg_num_y]);
    gp_regs_[reg_num_x] = gp_regs_[reg_num_y] - gp_regs_[reg_num_x];
    gp_regs_[0xF] = borrow_flag;
}

void CPU::op_8xyE(uint16_t reg_num_x, uint16_t reg_num_y) {
    uint16_t carry_bit = (gp_regs_[reg_num_x] & 0b10000000) ? 1 : 0;
    gp_regs_[reg_num_x] = gp_regs_[reg_num_y] << 1;
    gp_regs_[0xF] = carry_bit;
}

void CPU::op_9xy0(uint16_t reg_num_x, uint16_t reg_num_y) {
    if (gp_regs_[reg_num_x] != gp_regs_[reg_num_y]) {
        increment_pc_();
    }
}

void CPU::op_Annn(uint16_t opcode_data) {
    index_reg_ = opcode_data;
}

void CPU::op_Bnnn(uint16_t opcode_data) {
    increment_pc_flag_ = false;
    pc = gp_regs_[0x0] + opcode_data;
}

void CPU::op_Cxnn(uint16_t reg_num, uint16_t value) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> random_number(0, 255);

    gp_regs_[reg_num] = (random_number(gen) & value);
}

void CPU::op_Dxyn(uint16_t opcode_data, uint16_t reg_num_x, uint16_t reg_num_y, Memory& mem, GPU& gpu) {
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
}

void CPU::op_Ex9E(uint16_t reg_num, uint16_t& key_pressed) {
    if (key_pressed == gp_regs_[reg_num]) {
        increment_pc_();
    }
}

void CPU::op_ExA1(uint16_t reg_num, uint16_t& key_pressed) {
    if (key_pressed != gp_regs_[reg_num]) {
        increment_pc_();
    }
}

void CPU::op_Fx07(uint16_t reg_num) {
    gp_regs_[reg_num] = delay_reg_;
}

//void CPU::op_Fx0A(uint16_t reg_num, uint16_t& key_pressed, sf::RenderWindow& window) {
//    sf::Event event;
//    while (window.waitEvent(event)) {
//        if (event.type == sf::Event::Closed) {
//            window.close();
//        }
//        if (event.type == sf::Event::KeyPressed) {
//            gp_regs_[reg_num] = key_pressed;
//        }
//        else if (event.type == sf::Event::KeyReleased) {
//            break;
//        }
//    }
//}

void CPU::op_Fx15(uint16_t reg_num) {
    delay_reg_ = gp_regs_[reg_num];
}

void CPU::op_Fx18(uint16_t reg_num) {
    sound_reg_ = gp_regs_[reg_num];
}

void CPU::op_Fx1E(uint16_t reg_num) {
    index_reg_ += gp_regs_[reg_num];
}

void CPU::op_Fx29(uint16_t reg_num) {
    index_reg_ = gp_regs_[reg_num] * 0x5;
}

void CPU::op_Fx33(uint16_t reg_num, Memory& mem) {
    mem.memory[index_reg_] = (gp_regs_[reg_num] / 100);
    mem.memory[index_reg_ + 1] = ((gp_regs_[reg_num] % 100) / 10);
    mem.memory[index_reg_ + 2] = (gp_regs_[reg_num] % 10);
}

void CPU::op_Fx55(uint16_t reg_num, Memory& mem) {
    for (uint16_t i = 0; i <= reg_num; i++) {
        mem.memory[index_reg_ + i] = gp_regs_[i];
    }
    index_reg_ += reg_num + 1;
}

void CPU::op_Fx65(uint16_t reg_num, Memory& mem) {
    for (uint16_t i = 0; i <= reg_num; i++) {
        gp_regs_[i] = mem.memory[index_reg_ + i];
    }
    index_reg_ += reg_num + 1;
}

void CPU::decode_execute_opcode(Memory& mem, GPU& gpu, SDL_Renderer* renderer, uint16_t& key_pressed) {
    increment_pc_flag_ = true;
    uint16_t opcode_data = (opcode_ & 0xFFF);

    uint16_t reg_num = (opcode_data & 0xF00) >> 8;
    uint16_t reg_num_x = (opcode_data & 0xF00) >> 8;
    uint16_t reg_num_y = (opcode_data & 0xF0) >> 4;

    uint16_t value = (opcode_data & 0xFF);

    switch (opcode_ & 0xF000) {
    case 0x1000: {
        op_1nnn(opcode_data);
        break;
    }

    case 0x2000: {
        op_2nnn(opcode_data);
        break;
    }

    case 0x3000: {
        op_3xnn(reg_num, value);
        break;
    }

    case 0x4000: {
        op_4xnn(reg_num, value);
        break;
    }

    case 0x5000: {
        op_5xy0(reg_num_x, reg_num_y);
        break;
    }

    case 0x6000: {
        op_6xnn(reg_num, value);
        break;
    }

    case 0x7000: {
        op_7xnn(reg_num, value);
        break;
    }

    case 0x8000: {
        switch (opcode_data & 0xF) {
        case 0x0: {
            op_8xy0(reg_num_x, reg_num_y);
            break;
        }
        case 0x1: {
            op_8xy1(reg_num_x, reg_num_y);
            break;
        }
        case 0x2: {
            op_8xy2(reg_num_x, reg_num_y);
            break;
        }
        case 0x3: {
            op_8xy3(reg_num_x, reg_num_y);
            break;
        }
        case 0x4: {
            op_8xy4(reg_num_x, reg_num_y);
            break;
        }
        case 0x5: {
            op_8xy5(reg_num_x, reg_num_y);
            break;
        }
        case 0x6: {
            op_8xy6(reg_num_x, reg_num_y);
            break;
        }
        case 0x7: {
            op_8xy7(reg_num_x, reg_num_y);
            break;
        }
        case 0xE: {
            op_8xyE(reg_num_x, reg_num_y);
            break;
        }
        default: {
            throw std::runtime_error("Unknown opcode: " + (std::stringstream{} << std::hex << std::showbase << opcode_).str());
            break;
        }
        }
        break;
    }

    case 0x9000: {
        op_9xy0(reg_num_x, reg_num_y);
        break;
    }

    case 0xA000: {
        op_Annn(opcode_data);
        break;
    }

    case 0xB000: {
        op_Bnnn(opcode_data);
        break;
    }

    case 0xC000: {
        op_Cxnn(reg_num, value);
        break;
    }

    case 0xD000: {
        op_Dxyn(opcode_data, reg_num_x, reg_num_y, mem, gpu);
        break;
    }

    case 0xE000: {
        switch (opcode_data & 0xFF) {
        case 0x9E: {
            op_Ex9E(reg_num, key_pressed);
            break;
        }
        case 0xA1: {
            op_ExA1(reg_num, key_pressed);
            break;
        }

        default: {
            throw std::runtime_error("Unknown opcode: " + (std::stringstream{} << std::hex << std::showbase << opcode_).str());
            break;
        }
        }
        break;
    }

    case 0xF000: {
        switch (opcode_data & 0xFF) {
        case 0x07: {
            op_Fx07(reg_num);
            break;
        }
        //case 0x0A: {
        //    op_Fx0A(reg_num, key_pressed, window);
        //    break;
        //}
        case 0x15: {
            op_Fx15(reg_num);
            break;
        }
        case 0x18: {
            op_Fx18(reg_num);
            break;
        }
        case 0x1E: {
            op_Fx1E(reg_num);
            break;
        }
        case 0x29: {
            op_Fx29(reg_num);
            break;
        }
        case 0x33: {
            op_Fx33(reg_num, mem);
            break;
        }
        case 0x55: {
            op_Fx55(reg_num, mem);
            break;
        }
        case 0x65: {
            op_Fx65(reg_num, mem);
            break;
        }
        default: {
            throw std::runtime_error("Unknown opcode: " + (std::stringstream{} << std::hex << std::showbase << opcode_ ).str());
            break;
        }
        }
        break;
    }

    default:
        switch (opcode_) {
        case 0xEE:
            op_00EE();
            break;
        case 0xE0:
            op_00E0(gpu);
            break;
        default:
            throw std::runtime_error("Unknown opcode: " + (std::stringstream{} << std::hex << std::showbase << opcode_).str());
            break;
        }
    }

    if (increment_pc_flag_) {
        increment_pc_();
    }
}