#include "main.h"
#include "gpu.h"
#include "memory.h"
#include "cpu.h"

constexpr uint16_t INSTRUCTIONS_PER_FRAME = 11;

int wWinMain() {
    GPU gpu;
    Memory mem;
    CPU cpu;
    
    mem.load_game();

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
