#include "main.h"
#include "cpu.h"
#include "gpu.h"
#include "memory.h"

constexpr uint16_t INSTRUCTIONS_PER_FRAME = 11;

void load_game(Memory& mem, CPU& cpu);

int wWinMain() {
    CPU cpu;
    GPU gpu;
    Memory mem;
    
    load_game(mem, cpu);

    sf::RenderWindow window(sf::VideoMode(1280, 640), "Chip8 emulator");
    window.setFramerateLimit(60);
    window.clear();
    window.display();

    sf::SoundBuffer buffer;
    sf::Sound sound;
    sound.setVolume(30);
    buffer.loadFromFile("./assets/beep-1200.wav");

    uint16_t instructions_ran = 0;

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        uint16_t opcode = cpu.fetch_opcode(mem);
        cpu.execute_opcode(opcode, mem, gpu, window);

        instructions_ran += 1;

        gpu.render_frame_buffer(window);

        if (instructions_ran >= INSTRUCTIONS_PER_FRAME) {

            if (cpu.delay_reg > 0) {
                cpu.delay_reg -= 1;
            }

            if (cpu.sound_reg > 0) {
                sound.setBuffer(buffer);
                sound.play();

                cpu.sound_reg -= 1;
            }

            instructions_ran = 0;
            window.display();
        }
    }

    return 0;
}

void load_game(Memory& mem, CPU& cpu) {
    std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/games/Brick (Brix hack, 1990).ch8");
    //std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/demos/Particle Demo [zeroZshadow, 2008].ch8");
    //std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/chip8-test-suite-main/bin/7-beep.ch8");
    std::ifstream file(game_path, std::ios::binary);

    if (file.is_open()) {
        uintmax_t game_size = std::filesystem::file_size(game_path);

        for (uint16_t i = 0; i < game_size; i++) {
            file.read(reinterpret_cast<char*>(&mem.memory[cpu.pc + i]), 1);
        }
    }
}
