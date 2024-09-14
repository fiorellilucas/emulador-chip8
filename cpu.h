#include <iostream>
#include <stack>
#include <random>
#include <SFML/Graphics.hpp>

class GPU;
class Memory;

class CPU {
public:
    CPU();
    ~CPU();

    uint16_t pc = PROGRAM_START_ADDRESS;
    uint16_t delay_reg = 0;
    uint16_t sound_reg = 0;

    void execute_opcode(uint16_t& opcode, Memory& mem, GPU& gpu, sf::RenderWindow& window, uint16_t& key_pressed);

private:
    uint8_t gp_regs[16] = { 0 };
    uint16_t index_reg = 0;
    uint16_t sp;

    std::stack<uint16_t> stack;

    bool increment_pc_flag = true;

    void increment_pc();

};