#include "emulator.h"

Emulator::Emulator() {
    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 640), "Chip-8 emulator");
    window->setFramerateLimit(60);
    window->clear();
    
    buffer_.loadFromFile("./assets/beep-1200.wav");
    sound = std::make_unique<sf::Sound>();
    sound->setBuffer(buffer_);
    sound->setVolume(30);

    font_.loadFromFile("./assets/VT323-Regular.ttf");
    text_.setFont(font_);
    text_.setFillColor(sf::Color::White);
    text_.setCharacterSize(56);

    text_.setString("Chip-8 emulator");

    window->draw(text_);
    window->display();
}

uint16_t Emulator::decode_key_pressed() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) {
        return 0x0;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
        return 0x1;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
        return 0x2;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
        return 0x3;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
        return 0x4;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
        return 0x5;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)) {
        return 0x6;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7)) {
        return 0x7;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8)) {
        return 0x8;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) {
        return 0x9;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        return 0xA;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
        return 0xB;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
        return 0xC;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        return 0xD;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        return 0xE;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
        return 0xF;
    }
    else {
        return NULL;
    }
}