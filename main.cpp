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

            if (event.key.code == sf::Keyboard::Escape) {
                emulator.quit_game();
            }

            if (event.key.code == sf::Keyboard::R) {
                emulator.reload_game();
            }
        }

        if (emulator.game_is_loaded) {
            // EMULATION CYCLE

            uint16_t key_pressed = emulator.decode_key_pressed();

            emulator.cpu->fetch_opcode(*emulator.mem);
            emulator.cpu->decode_execute_opcode(*emulator.mem, *emulator.gpu, *emulator.window, key_pressed);

            emulator.gpu->render_frame_buffer(*emulator.window);

            emulator.instructions_ran += 1;
            if (emulator.instructions_ran >= INSTRUCTIONS_PER_FRAME) {
                if (emulator.cpu->get_delay() > 0) {
                    emulator.cpu->decrement_delay();
                }

                if (emulator.cpu->get_sound() > 0) {
                    emulator.sound->play();
                    emulator.cpu->decrement_sound();
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
