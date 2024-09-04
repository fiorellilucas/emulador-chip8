#include "chip8.h"

Chip8::Chip8() {
    for (uint16_t mem_index = 0; mem_index < 80; mem_index++) {
        memory[mem_index] = fontset[mem_index];
    }
}

void Chip8::change_clock(double& clock) {
    clock_hz = clock;
    std::chrono::duration<double, std::milli>new_period{ (1.0 / clock_hz) * 1000 };
    instr_full_period = new_period;
}

void Chip8::increment_pc() {
    pc += 2;
}

void Chip8::sleep_remaining_period(auto& start_exec_time, auto& end_exec_time) {
    auto instr_time_taken = std::chrono::duration<double, std::milli>{ end_exec_time - start_exec_time };
    auto remaining_instr_period = std::chrono::duration<double, std::milli>{ instr_full_period - instr_time_taken };

    std::this_thread::sleep_for(remaining_instr_period);
}

void Chip8::delay_timer(uint16_t& reg_num) {
    delay_reg = gp_regs[reg_num];

    change_clock(TIMER_EXEC_HZ);
    std::this_thread::sleep_for(instr_full_period * delay_reg);
    change_clock(NORMAL_EXEC_HZ);

    delay_reg = 0;
}

void Chip8::sound_timer(uint16_t& reg_num) {
    sound_reg = gp_regs[reg_num];

    change_clock(TIMER_EXEC_HZ);
    std::this_thread::sleep_for(instr_full_period * sound_reg);
    change_clock(NORMAL_EXEC_HZ);

    sound_reg = 0;

    // MAKE SOUND
}


void Chip8::execute_opcode(uint16_t& opcode) {
    auto start_exec_time = std::chrono::high_resolution_clock::now();

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
            uint16_t previous_value = gp_regs[reg_num_x];
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
            uint16_t previous_value = gp_regs[reg_num_x];
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
            uint16_t previous_value = gp_regs[reg_num_x];
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
            gp_regs[reg_num] = delay_reg;
            break;
        }
        case 0x0A: {
            break;
        }
        case 0x15: {
            delay_timer(reg_num);
            break;
        }
        case 0x18: {
            sound_timer(reg_num);
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
            memory[index_reg] = (gp_regs[reg_num] & 0b111100000000);
            memory[index_reg + 1] = (gp_regs[reg_num] & 0b11110000);
            memory[index_reg + 2] = (gp_regs[reg_num] & 0b1111);
            break;
        }
        case 0x55: {
            for (uint16_t i = 0; i <= reg_num; i++) {
                memory[index_reg + i] = gp_regs[i];
            }
            index_reg += reg_num + 1;
            break;
        }
        case 0x65: {
            for (uint16_t i = 0; i <= reg_num; i++) {
                gp_regs[i] = memory[index_reg + i];
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

    default:
        switch (opcode) {
        case 0xEE:
            pc = stack.top();
            stack.pop();
            break;
        case 0xE0:
            break;
        default:
            std::cout << std::hex << std::showbase << "opcode " << opcode << " unknown\n";
            break;
        }
    }

    if (increment_pc_flag) {
        increment_pc();
    }

    auto end_exec_time = std::chrono::high_resolution_clock::now();
    sleep_remaining_period(start_exec_time, end_exec_time);
}