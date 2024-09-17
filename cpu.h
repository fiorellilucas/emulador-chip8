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

    uint16_t get_delay();
    uint16_t get_sound();
    void decrement_delay();
    void decrement_sound();
    void execute_opcode(uint16_t& opcode, Memory& mem, GPU& gpu, sf::RenderWindow& window, uint16_t& key_pressed);

private:
    uint8_t gp_regs_[16] = { 0 };
    uint16_t index_reg_ = 0;
    uint16_t delay_reg_ = 0;
    uint16_t sound_reg_ = 0;
    uint16_t sp_;
    std::stack<uint16_t> stack_;
    bool increment_pc_flag_ = true;

    void increment_pc_();

};