#include <iostream>
#include <stack>
#include <random>
#include <SFML/Graphics.hpp>

class GPU;
class Memory;

class CPU {
public:
    CPU();

    uint16_t pc = PROGRAM_START_ADDRESS;
    uint16_t delay_reg = 0;
    uint16_t sound_reg = 0;

    uint16_t fetch_opcode(Memory& mem) const;
    void execute_opcode(uint16_t& opcode, Memory& mem, GPU& gpu, sf::RenderWindow& window);

private:
    uint8_t gp_regs[16] = { 0 };
    uint16_t index_reg = 0;
    uint16_t sp;

    std::stack<uint16_t> stack;

    bool increment_pc_flag = true;

    void increment_pc();
    int decode_key_pressed();
};