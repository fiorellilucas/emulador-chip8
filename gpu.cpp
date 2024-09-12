#include "gpu.h"

GPU::GPU() {}

void GPU::draw_pixel(uint16_t& pixel_state, uint16_t& pixel_pos_x, uint16_t& pixel_pos_y, sf::RenderWindow& window) {
    sf::RectangleShape sprite_pixel(sf::Vector2f(20, 20));

    if (pixel_state) {
        sprite_pixel.setFillColor(sf::Color(255, 255, 255));
    }
    else {
        sprite_pixel.setFillColor(sf::Color(0, 0, 0));
    }

    sprite_pixel.setPosition(sf::Vector2f(
        (pixel_pos_x * RES_SCALING),
        (pixel_pos_y * RES_SCALING)
    ));

    window.draw(sprite_pixel);
}

void GPU::render_frame_buffer(sf::RenderWindow& window) {
    for (uint16_t pixel_pos_y = 0; pixel_pos_y < 32; pixel_pos_y++) {
        for (uint16_t pixel_pos_x = 0; pixel_pos_x < 64; pixel_pos_x++) {
            draw_pixel(frame_buffer[pixel_pos_y][pixel_pos_x], pixel_pos_x, pixel_pos_y, window);
        }
    }
}