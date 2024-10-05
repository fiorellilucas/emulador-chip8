#include "main.h"
//#include "gpu.h"
//#include "memory.h"
//#include "cpu.h"
#include "emulator.h"

constexpr uint16_t INSTRUCTIONS_PER_FRAME = 11;

int main(int argc, char** args) {
    Emulator emulator;

    while (emulator.running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                SDL_DestroyWindow(emulator.window);
                SDL_Quit();
                emulator.running = false;
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    //emulator.quit_game();
                    break;
                case SDLK_r:
                    //emulator.reload_game();
                    break;  
                default:
                    break;
                }
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_w:
                    emulator.cursor_position > 0 ? emulator.cursor_position -= 1 : NULL;
                    break;
                case SDLK_s:
                    emulator.cursor_position < (emulator.num_games_installed() - 1) ? emulator.cursor_position += 1 : NULL;
                    break;
                case SDLK_RETURN:
                    //emulator.game_is_loaded = emulator.mem->load_game(emulator.game_selected());
                    break;
                default:
                    emulator.key_pressed = emulator.decode_key_pressed(event);
                    break;
                }
            }

            if (emulator.game_is_loaded) {
                // EMULATION CYCLE

                //emulator.cpu->fetch_opcode(*emulator.mem);

                //try {
                //    emulator.cpu->decode_execute_opcode(*emulator.mem, *emulator.gpu, *emulator.window, key_pressed);
                //}
                //catch (std::runtime_error exception) {
                //    emulator.error_message(exception);
                //}

                //emulator.gpu->render_frame_buffer(*emulator.window);

                //emulator.instructions_ran += 1;
                //if (emulator.instructions_ran >= INSTRUCTIONS_PER_FRAME) {
                //    if (emulator.cpu->get_delay() > 0) {
                //        emulator.cpu->decrement_delay();
                //    }

                //    if (emulator.cpu->get_sound() > 0) {
                //        emulator.sound->play();
                //        emulator.cpu->decrement_sound();
                //    }

                //    emulator.instructions_ran = 0;
                //    emulator.window->display();
                //}
            }
            else {
                // GAME SELECTION        
                emulator.list_games();
            }
        }
    }
    return 0;
}
