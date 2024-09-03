#include <iostream>
#include <stack>
#include <random>
#include <chrono>
#include <thread>

class Chip8 {
private:
    double clock_hz = 480.0;

    uint16_t PROGRAM_START_ADDRESS = 0x200;
    uint16_t gp_regs[16] = { 0 };
    uint16_t index_reg = 0;
    uint16_t sp;

    std::stack<uint16_t> stack;
    std::chrono::duration<double, std::milli> instr_full_period{ (1.0 / clock_hz) * 1000 };

    bool increment_pc_flag = true;

    void sleep_remaining_period(auto& start_exec_time, auto& end_exec_time);
    void increment_pc();

public:
    uint16_t memory[4096] = { 0 };
    uint16_t pc = PROGRAM_START_ADDRESS;

    Chip8();

    void execute_opcode(uint16_t& opcode);
};