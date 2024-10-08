#include "gpu.h"

GPU::GPU() {}

GPU::~GPU() {}

void GPU::draw_pixel(uint16_t& pixel_state, uint16_t& pixel_pos_x, uint16_t& pixel_pos_y, SDL_Renderer* renderer) {
    SDL_Rect pixel = { pixel_pos_x * RES_SCALING, pixel_pos_y * RES_SCALING, 20, 20 };

    if (pixel_state) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    }

    SDL_RenderFillRect(renderer, &pixel);
}

void GPU::render_frame_buffer(SDL_Renderer* renderer) {
    for (uint16_t pixel_pos_y = 0; pixel_pos_y < 32; pixel_pos_y++) {
        for (uint16_t pixel_pos_x = 0; pixel_pos_x < 64; pixel_pos_x++) {
            draw_pixel(frame_buffer[pixel_pos_y][pixel_pos_x], pixel_pos_x, pixel_pos_y, renderer);
        }
    }
    SDL_RenderPresent(renderer);
}