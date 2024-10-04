#include "memory.h"
#include "cpu.h"
#include "gpu.h"
#include "emulator.h"

Emulator::Emulator() {
    cpu = std::make_unique<CPU>();
    mem = std::make_unique<Memory>();
    gpu = std::make_unique<GPU>();

    window = std::make_unique<sf::RenderWindow>(sf::VideoMode(1280, 640), "Chip-8 emulator");
    window->setFramerateLimit(60);
    window->clear();

    buffer_.loadFromFile("./assets/beep-1200.wav");
    sound = std::make_unique<sf::Sound>();
    sound->setBuffer(buffer_);
    sound->setVolume(30);

    font_.loadFromFile("./assets/font.ttf");
    text_.setFont(font_);
    text_.setFillColor(sf::Color::White);

    // fills the game_entries_ vector with all the roms (std::filesystem::directory_entry's)
    if (!std::filesystem::is_empty(games_path_)) {
        for (auto const& rom : std::filesystem::directory_iterator{ games_path_ })
            if (rom.path().extension() == ".ch8") {
                games_entries_.push_back(rom);
            }
    }

    list_games();
    window->display();
}

Emulator::~Emulator() {}

void Emulator::draw_control_glyphs_() {
    sf::Font glyph_font;
    glyph_font.loadFromFile("./assets/glyphs.otf");

    sf::Text glyph;
    glyph.setFont(glyph_font);
    glyph.setFillColor(sf::Color::White);
    glyph.setCharacterSize(60);

    glyph.setString("W");
    glyph.setPosition(1000, 20);
    window->draw(glyph);

    glyph.setString("S");
    glyph.setPosition(1000, 80);
    window->draw(glyph);

    glyph.setString("R");
    glyph.setPosition(1000, 140);
    window->draw(glyph);

    glyph.setString("m");
    glyph.setPosition(1000, 200);
    window->draw(glyph);

    glyph.setFont(font_);
    glyph.setCharacterSize(22);

    glyph.setString("GO UP");
    glyph.setPosition(1060, 42);
    window->draw(glyph);

    glyph.setString("GO DOWN");
    glyph.setPosition(1060, 102);
    window->draw(glyph);

    glyph.setString("RESET GAME");
    glyph.setPosition(1060, 162);
    window->draw(glyph);

    glyph.setString("QUIT TO MENU");
    glyph.setPosition(1060, 222);
    window->draw(glyph);
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

uint16_t Emulator::num_games_installed() {
    return games_entries_.size();
}

void Emulator::draw_logo_() {
    text_.setFillColor(sf::Color::White);
    text_.setCharacterSize(54);
    text_.setPosition(20, 20);
    text_.setString("Chip-8 emulator");
    window->draw(text_);
}

void Emulator::list_games() {
    draw_logo_();
    draw_control_glyphs_();

    text_.setCharacterSize(28);
    uint16_t line_pos_x = 20;
    uint16_t line_pos_y = 110;

    if (games_entries_.empty()) {
        text_.setString("No roms available");
        text_.setPosition(sf::Vector2f(line_pos_x, line_pos_y));
        window->draw(text_);

        return;
    }

    uint16_t page_start = (cursor_position / GAMES_PER_PAGE_) * GAMES_PER_PAGE_;
    for (uint16_t i = page_start; i < (page_start + GAMES_PER_PAGE_); i++) {
        if (i >= num_games_installed()) {
            break;
        }

        if (i == cursor_position) {
            text_.setFillColor(sf::Color::Cyan);
        }
        else {
            text_.setFillColor(sf::Color::White);
        }

        text_.setString(sf::String(games_entries_[i].path().stem()));
        text_.setPosition(sf::Vector2f(line_pos_x, line_pos_y));
        window->draw(text_);

        line_pos_y += 50;
    }
}

std::filesystem::directory_entry Emulator::game_selected() {
    return games_entries_[cursor_position];
}

void Emulator::quit_game() {
    reset_system();
    game_is_loaded = false;
}

void Emulator::reload_game() {
    quit_game();
    game_is_loaded = mem->load_game(game_selected());
}

void Emulator::reset_system() {
    cpu = std::make_unique<CPU>();
    mem = std::make_unique<Memory>();
    gpu = std::make_unique<GPU>();
}