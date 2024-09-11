#include "main.h"
#include "chip8.h"

constexpr uint16_t RES_SCALING = 20;
constexpr uint16_t INSTRUCTIONS_PER_FRAME = 11;

void load_game(Chip8& chip8);
void draw_pixel(uint16_t& pixel_state, uint16_t& pixel_pos_x, uint16_t& pixel_pos_y, sf::RenderWindow& window);
void render_frame_buffer(uint16_t frame_buffer[32][64], sf::RenderWindow& window);

static int WinMain() {
    Chip8 chip8;
    load_game(chip8);

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

        uint16_t opcode = chip8.fetch_opcode();
        chip8.execute_opcode(opcode, window);

        instructions_ran += 1;

        render_frame_buffer(chip8.frame_buffer, window);

        if (instructions_ran >= INSTRUCTIONS_PER_FRAME) {

            if (chip8.delay_reg > 0) {
                chip8.delay_reg -= 1;
            }

            if (chip8.sound_reg > 0) {
                sound.setBuffer(buffer);
                sound.play();

                chip8.sound_reg -= 1;
            }

            instructions_ran = 0;
            window.display();
        }
    }

    return 0;
}

void load_game(Chip8& chip8) {
    std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/games/Pong.ch8");
    //std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/demos/Particle Demo [zeroZshadow, 2008].ch8");
    //std::filesystem::path game_path("C:/Users/lucas/Documents/chip8-roms/chip8-test-suite-main/bin/7-beep.ch8");
    std::ifstream file(game_path, std::ios::binary);

    if (file.is_open()) {
        uintmax_t game_size = std::filesystem::file_size(game_path);

        for (uint16_t i = 0; i < game_size; i++) {
            file.read(reinterpret_cast<char*>(&chip8.memory[chip8.pc + i]), 1);
        }
    }
}

void draw_pixel(uint16_t& pixel_state, uint16_t& pixel_pos_x, uint16_t& pixel_pos_y, sf::RenderWindow& window) {
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

void render_frame_buffer(uint16_t frame_buffer[32][64], sf::RenderWindow& window) {
    for (uint16_t pixel_pos_y = 0; pixel_pos_y < 32; pixel_pos_y++) {
        for (uint16_t pixel_pos_x = 0; pixel_pos_x < 64; pixel_pos_x++) {
            draw_pixel(frame_buffer[pixel_pos_y][pixel_pos_x], pixel_pos_x, pixel_pos_y, window);
        }
    }
}