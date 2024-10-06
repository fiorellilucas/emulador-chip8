#include <iostream>
#include <stack>
#include <random>
#include <exception>
#include <SDL.h>

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

    void fetch_opcode(Memory& mem);
    void decode_execute_opcode(Memory& mem, GPU& gpu, SDL_Renderer* renderer, uint16_t& key_pressed);

private:
    uint8_t gp_regs_[16] = { 0 };
    uint16_t index_reg_ = 0;
    uint16_t delay_reg_ = 0;
    uint16_t sound_reg_ = 0;
    uint16_t sp_;
    uint16_t opcode_ = 0x0;
    std::stack<uint16_t> stack_;
    bool increment_pc_flag_ = true;

    void increment_pc_();
    void set_opcode_(uint16_t opcode);

    //void op_00E0(GPU& gpu);
    void op_00EE();
    void op_1nnn(uint16_t opcode_data);
    void op_2nnn(uint16_t opcode_data);
    void op_3xnn(uint16_t reg_num, uint16_t value);
    void op_4xnn(uint16_t reg_num, uint16_t value);
    void op_5xy0(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_6xnn(uint16_t reg_num, uint16_t value);
    void op_7xnn(uint16_t reg_num, uint16_t value);
    void op_8xy0(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_8xy1(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_8xy2(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_8xy3(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_8xy4(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_8xy5(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_8xy6(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_8xy7(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_8xyE(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_9xy0(uint16_t reg_num_x, uint16_t reg_num_y);
    void op_Annn(uint16_t opcode_data);
    void op_Bnnn(uint16_t opcode_data);
    void op_Cxnn(uint16_t reg_num, uint16_t value);
    void op_Dxyn(uint16_t opcode_data, uint16_t reg_num_x, uint16_t reg_num_y, Memory& mem, GPU& gpu);
    void op_Ex9E(uint16_t reg_num, uint16_t& key_pressed);
    void op_ExA1(uint16_t reg_num, uint16_t& key_pressed);
    void op_Fx07(uint16_t reg_num);
    //void op_Fx0A(uint16_t reg_num, uint16_t& key_pressed, sf::RenderWindow& window);
    void op_Fx15(uint16_t reg_num);
    void op_Fx18(uint16_t reg_num);
    void op_Fx1E(uint16_t reg_num);
    void op_Fx29(uint16_t reg_num);
    void op_Fx33(uint16_t reg_num, Memory& mem);
    void op_Fx55(uint16_t reg_num, Memory& mem);
    void op_Fx65(uint16_t reg_num, Memory& mem);

};