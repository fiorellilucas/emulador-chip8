#include <iostream>
#include <stack>
#include <random>
#include <chrono>
#include <thread>

class Chip8 {
private:
    double NORMAL_EXEC_HZ = 480.0;
    double TIMER_EXEC_HZ = 60.0;

    double clock_hz = NORMAL_EXEC_HZ;

    uint16_t PROGRAM_START_ADDRESS = 0x200;
    uint16_t gp_regs[16] = { 0 };
    uint16_t index_reg = 0;
    uint16_t delay_reg = 0;
    uint16_t sound_reg = 0;
    uint16_t sp;

    std::stack<uint16_t> stack;
    std::chrono::duration<double, std::milli> instr_full_period{ (1.0 / clock_hz) * 1000 };

    uint16_t fontset[80] = {
        0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xf0, 0x10, 0xf0, 0x80, 0xf0, // 2
        0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
        0x90, 0x90, 0xf0, 0x10, 0x10, // 4
        0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
        0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
        0xf0, 0x10, 0x20, 0x40, 0x40, // 7
        0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
        0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
        0xf0, 0x90, 0xf0, 0x90, 0x90, // A
        0xe0, 0x90, 0xe0, 0x90, 0xe0, // B
        0xf0, 0x80, 0x80, 0x80, 0xf0, // C
        0xe0, 0x90, 0x90, 0x90, 0xe0, // D
        0xf0, 0x80, 0xf0, 0x80, 0xf0, // E
        0xf0, 0x80, 0xf0, 0x80, 0x80  // F
    };

    bool increment_pc_flag = true;

    void sleep_remaining_period(auto& start_exec_time, auto& end_exec_time);
    void increment_pc();
    void change_clock(double& clock);
    void delay_timer(uint16_t& reg_num);
    void sound_timer(uint16_t& reg_num);

public:
    uint16_t memory[4096] = { 0 };
    uint16_t pc = PROGRAM_START_ADDRESS;

    Chip8();

    uint16_t fetch_opcode() const;
    void execute_opcode(uint16_t& opcode);

};