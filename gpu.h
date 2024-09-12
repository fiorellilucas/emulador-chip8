#include <iostream>
#include <SFML/Graphics.hpp>

constexpr uint16_t RES_SCALING = 20;

class GPU {
public:
    GPU();

    uint16_t frame_buffer[32][64] = { 0 };

    void draw_pixel(uint16_t& pixel_state, uint16_t& pixel_pos_x, uint16_t& pixel_pos_y, sf::RenderWindow& window);
    void render_frame_buffer(sf::RenderWindow& window);
};