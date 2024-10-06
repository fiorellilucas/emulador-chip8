#include "main.h"
#include "gpu.h"
#include "memory.h"
#include "cpu.h"
#include "emulator.h"

constexpr uint16_t INSTRUCTIONS_PER_FRAME = 11;
constexpr double TARGET_FRAME_TIME = 16.6667;

int main(int argc, char** args) {
    Emulator emulator;

    while (emulator.running) {
        Uint64 start = SDL_GetPerformanceCounter();

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                SDL_DestroyRenderer(emulator.renderer);
                SDL_DestroyWindow(emulator.window);
                SDL_Quit();
                emulator.running = false;
                return 0;
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    emulator.quit_game();
                    break;
                case SDLK_r:
                    emulator.reload_game();
                    break;
                case SDLK_RETURN:
                    emulator.game_is_loaded = emulator.mem->load_game(emulator.game_selected());
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
                default:
                    break;
                }
            }
        }
        if (emulator.game_is_loaded) {
            // EMULATION CYCLE
            
            uint16_t key_pressed = emulator.decode_key_pressed();
            emulator.cpu->fetch_opcode(*emulator.mem);

            try {
                emulator.cpu->decode_execute_opcode(*emulator.mem, *emulator.gpu, emulator.renderer, key_pressed);
            }
            catch (std::runtime_error exception) {
                //emulator.error_message(exception);
            }

            emulator.gpu->render_frame_buffer(emulator.renderer);

            emulator.instructions_ran += 1;
            if (emulator.instructions_ran >= INSTRUCTIONS_PER_FRAME) {
                if (emulator.cpu->get_delay() > 0) {
                    emulator.cpu->decrement_delay();
                }

                if (emulator.cpu->get_sound() > 0) {
                    std::cout << emulator.cpu->get_sound() << std::endl;
                    if (!Mix_Playing(1)) {
                        Mix_PlayChannel(1, emulator.sfx, -1);
                    }
                    emulator.cpu->decrement_sound();
                }
                else {
                    Mix_HaltChannel(1);
                }

                emulator.instructions_ran = 0;
                SDL_RenderPresent(emulator.renderer);

                Uint64 end = SDL_GetPerformanceCounter();
                double elapsed_time = (double)(end - start) / (double)SDL_GetPerformanceFrequency() * 1000.0f;

                double remaining_frame_time = TARGET_FRAME_TIME - elapsed_time;                

                if (remaining_frame_time > 0) {
                    SDL_Delay(static_cast<Uint32>(remaining_frame_time));
                }
            }
        }
        else {
            // GAME SELECTION        
            emulator.list_games();
        }
    }
    return 0;
}
