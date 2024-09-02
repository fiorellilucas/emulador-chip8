#include <iostream>
#include <stack>
#include <random>

class Chip8 {
private:
    uint16_t PROGRAM_START_ADDRESS = 0x200;

public:
    uint8_t gp_regs[16] = { 0 };
    uint16_t memory[4096] = { 0 };
    uint16_t index_reg = 0;
    uint16_t pc = PROGRAM_START_ADDRESS;
    uint16_t sp;
    bool increment_pc_flag = true;
    std::stack<uint16_t> stack;

    Chip8();

    void execute_opcode(uint16_t& opcode);
    void increment_pc();
};