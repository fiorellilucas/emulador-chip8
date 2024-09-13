#include "emulator.h"

Emulator::Emulator() {
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 640), "Chip8 emulator");
    window->setFramerateLimit(60);
    window->clear();
    window->display();
    
    buffer_.loadFromFile("./assets/beep-1200.wav");
    sound = std::make_unique<sf::Sound>();
    sound->setBuffer(buffer_);
    sound->setVolume(30);
}