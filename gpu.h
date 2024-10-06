#include <iostream>
#include <SDL.h>

constexpr uint16_t RES_SCALING = 20;

class GPU {
public:
    GPU();
    ~GPU();

    uint16_t frame_buffer[32][64] = { 0 };

    void draw_pixel(uint16_t& pixel_state, uint16_t& pixel_pos_x, uint16_t& pixel_pos_y, SDL_Renderer* renderer);
    void render_frame_buffer(SDL_Renderer* renderer);
};