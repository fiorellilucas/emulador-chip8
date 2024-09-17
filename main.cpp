#include "main.h"
#include "gpu.h"
#include "memory.h"
#include "cpu.h"
#include "emulator.h"

constexpr uint16_t INSTRUCTIONS_PER_FRAME = 11;

int wWinMain() {
    Emulator emulator;

    while (emulator.window->isOpen()) {
        sf::Event event;
        while (emulator.window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                emulator.window->close();
            }
        }

        if (event.key.code == sf::Keyboard::Escape) {
            emulator.game_is_loaded = false;
            emulator.reset_system();
        }

        if (emulator.game_is_loaded) {
            // EMULATION CYCLE

            emulator.opcode = emulator.mem->fetch_opcode(emulator.cpu->pc);
            uint16_t key_pressed = emulator.decode_key_pressed();

            emulator.cpu->execute_opcode(emulator.opcode, *emulator.mem, *emulator.gpu, *emulator.window, key_pressed);
            emulator.gpu->render_frame_buffer(*emulator.window);

            emulator.instructions_ran += 1;
            if (emulator.instructions_ran >= INSTRUCTIONS_PER_FRAME) {
                if (emulator.cpu->delay_reg > 0) {
                    emulator.cpu->delay_reg -= 1;
                }

                if (emulator.cpu->sound_reg > 0) {
                    emulator.sound->play();
                    emulator.cpu->sound_reg -= 1;
                }

                emulator.instructions_ran = 0;
                emulator.window->display();
            }
        }
        else {
            // GAME SELECTION        
    
            emulator.window->clear();
            emulator.list_games();
            emulator.window->display();

            while (emulator.window->pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    emulator.window->close();
                }

                if (event.type == sf::Event::KeyPressed) {
                    if ((event.key.code == sf::Keyboard::W) || (event.key.code == sf::Keyboard::Up)) {
                        emulator.cursor_position > 0 ? emulator.cursor_position -= 1 : NULL;
                    }
                    else if ((event.key.code == sf::Keyboard::S) || (event.key.code == sf::Keyboard::Down)) {
                        emulator.cursor_position < (emulator.num_games_installed() - 1) ? emulator.cursor_position += 1 : NULL;
                    }
                    else if (event.key.code == sf::Keyboard::Enter) {
                        emulator.game_is_loaded = emulator.mem->load_game(emulator.game_selected());
                    }
                }
            }
        }
    }

    return 0;
}
