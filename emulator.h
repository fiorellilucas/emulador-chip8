#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Emulator {
public:
    Emulator();
    
    std::unique_ptr<sf::RenderWindow> window; 
    std::unique_ptr<sf::Sound> sound;

    bool game_is_loaded = false;
    uint16_t instructions_ran = 0;
    uint16_t opcode = 0x0;

private:
    sf::SoundBuffer buffer_;

};