#include "main.h"
#include "gpu.h"
#include "memory.h"
#include "cpu.h"
#include "emulator.h"

constexpr uint16_t INSTRUCTIONS_PER_FRAME = 11;

int wWinMain() {
    Emulator emulator;

    CPU cpu;
    Memory mem;
    GPU gpu;
    
    emulator.game_is_loaded = mem.load_game();

    while (emulator.window->isOpen()) {
        sf::Event event;
        while (emulator.window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                emulator.window->close();
            }
        }

        if (emulator.game_is_loaded) {
            emulator.opcode = mem.fetch_opcode(cpu.pc);

            cpu.execute_opcode(emulator.opcode, mem, gpu, *emulator.window);
            gpu.render_frame_buffer(*emulator.window);

            emulator.instructions_ran += 1;
            if (emulator.instructions_ran >= INSTRUCTIONS_PER_FRAME) {
                if (cpu.delay_reg > 0) {
                    cpu.delay_reg -= 1;
                }

                if (cpu.sound_reg > 0) {
                    emulator.sound->play();
                    cpu.sound_reg -= 1;
                }

                emulator.instructions_ran = 0;
                emulator.window->display();
            }
        }
    }

    return 0;
}
