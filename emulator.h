#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Emulator {
public:
    Emulator();
    ~Emulator();
    
    std::unique_ptr<sf::RenderWindow> window; 
    std::unique_ptr<sf::Sound> sound;

    bool game_is_loaded = false;
    uint16_t instructions_ran = 0;
    uint16_t opcode = 0x0;

    uint16_t decode_key_pressed();

private:
    sf::SoundBuffer buffer_;
    sf::Font font_;
    sf::Text text_;
};