//#include "memory.h"
//#include "cpu.h"
//#include "gpu.h"
#include "emulator.h"

Emulator::Emulator() {
    //cpu = std::make_unique<CPU>();
    //mem = std::make_unique<Memory>();
    //gpu = std::make_unique<GPU>();

    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Chip-8 emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 640, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << SDL_GetError() << std::endl;
    }
 
    if (TTF_Init() == -1) {
        std::cout << TTF_GetError() << std::endl;
    }

    font_ = TTF_OpenFont("./assets/font.ttf", 60);
    if (!font_) {
        std::cout << TTF_GetError() << std::endl;
    }

    draw_logo_();
    // fills the game_entries_ vector with all the roms (std::filesystem::directory_entry's)
    //if (!std::filesystem::is_empty(games_path_)) {
    //    for (auto const& rom : std::filesystem::directory_iterator{ games_path_ })
    //        if (rom.path().extension() == ".ch8") {
    //            games_entries_.push_back(rom);
    //        }
    //}
}

Emulator::~Emulator() {}

//void Emulator::draw_control_glyphs_() {
//    sf::Font glyph_font;
//    glyph_font.loadFromFile("./assets/glyphs.otf");
//
//    sf::Text glyph;
//    glyph.setFont(glyph_font);
//    glyph.setFillColor(sf::Color::White);
//    glyph.setCharacterSize(60);
//
//    glyph.setString("W");
//    glyph.setPosition(1000, 20);
//    window->draw(glyph);
//
//    glyph.setString("S");
//    glyph.setPosition(1000, 80);
//    window->draw(glyph);
//
//    glyph.setString("R");
//    glyph.setPosition(1000, 140);
//    window->draw(glyph);
//
//    glyph.setString("a");
//    glyph.setPosition(1000, 200);
//    window->draw(glyph);
//
//    glyph.setString("m");
//    glyph.setPosition(1000, 260);
//    window->draw(glyph);
//
//    glyph.setFont(font_);
//    glyph.setCharacterSize(22);
//
//    glyph.setString("GO UP");
//    glyph.setPosition(1060, 42);
//    window->draw(glyph);
//
//    glyph.setString("GO DOWN");
//    glyph.setPosition(1060, 102);
//    window->draw(glyph);
//
//    glyph.setString("RESET GAME");
//    glyph.setPosition(1060, 162);
//    window->draw(glyph);
//
//    glyph.setString("SELECT GAME");
//    glyph.setPosition(1080, 222);
//    window->draw(glyph);
//
//    glyph.setString("QUIT TO MENU");
//    glyph.setPosition(1060, 282);
//    window->draw(glyph);
//}

uint16_t Emulator::decode_key_pressed() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_0:
                return 0x0;
                break;
            case SDLK_1:
                return 0x1;
                break;
            case SDLK_2:
                return 0x2;
                break;
            case SDLK_3:
                return 0x3;
                break;
            case SDLK_4:
                return 0x4;
                break;
            case SDLK_5:
                return 0x5;
                break;
            case SDLK_6:
                return 0x6;
                break;
            case SDLK_7:
                return 0x7;
                break;
            case SDLK_8:
                return 0x8;
                break;
            case SDLK_9:
                return 0x9;
                break;
            case SDLK_a:
                return 0xA;
                break;
            case SDLK_b:
                return 0xB;
                break;
            case SDLK_c:
                return 0xC;
                break;
            case SDLK_d:
                return 0xD;
                break;
            case SDLK_e:
                return 0xE;
                break;
            case SDLK_f:
                return 0xF;
                break;
            default:
                return NULL;
                break;
            }
        }
    }
}

uint16_t Emulator::num_games_installed() {
    return games_entries_.size();
}

void Emulator::draw_logo_() {
    SDL_Color white = { 255, 255, 255 };

    text_surface_ = TTF_RenderText_Solid(font_, "Chip-8 Emulator", white);
    if (!text_surface_) {
        std::cout << TTF_GetError() << std::endl;
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface_);
    if (!text_texture) {
        std::cout << SDL_GetError() << std::endl;
    }
    SDL_FreeSurface(text_surface_);

    SDL_Rect text_rect = { 20, 20, 0, 0 };
    SDL_QueryTexture(text_texture, NULL, NULL, &text_rect.w, &text_rect.h);

    SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(text_texture);
}

//void Emulator::list_games() {
//    //draw_logo_();
//    //draw_control_glyphs_();
//
//    text_.setCharacterSize(28);
//    uint16_t line_pos_x = 20;
//    uint16_t line_pos_y = 110;
//
//    if (games_entries_.empty()) {
//        text_.setString("No roms available");
//        text_.setPosition(sf::Vector2f(line_pos_x, line_pos_y));
//        window->draw(text_);
//
//        return;
//    }
//
//    uint16_t page_start = (cursor_position / GAMES_PER_PAGE_) * GAMES_PER_PAGE_;
//    for (uint16_t i = page_start; i < (page_start + GAMES_PER_PAGE_); i++) {
//        if (i >= num_games_installed()) {
//            break;
//        }
//
//        if (i == cursor_position) {
//            text_.setFillColor(sf::Color::Cyan);
//        }
//        else {
//            text_.setFillColor(sf::Color::White);
//        }
//
//        text_.setString(sf::String(games_entries_[i].path().stem()));
//        text_.setPosition(sf::Vector2f(line_pos_x, line_pos_y));
//        window->draw(text_);
//
//        line_pos_y += 50;
//    }
//}

std::filesystem::directory_entry Emulator::game_selected() {
    return games_entries_[cursor_position];
}

//void Emulator::quit_game() {
//    reset_system();
//    game_is_loaded = false;
//}

//void Emulator::reload_game() {
//    quit_game();
//    game_is_loaded = mem->load_game(game_selected());
//}

//void Emulator::reset_system() {
//    cpu = std::make_unique<CPU>();
//    mem = std::make_unique<Memory>();
//    gpu = std::make_unique<GPU>();
//}

//void Emulator::error_message(std::runtime_error exception) {
//    sf::RenderWindow err_window(sf::VideoMode(320, 150), "Runtime error");
//
//    sf::Text text;
//    text.setFont(font_);
//    text.setCharacterSize(26);
//    text.setPosition(10, 30);
//    text.setString(exception.what());
//
//    err_window.clear();
//    err_window.draw(text);
//    err_window.display();
//
//    while (err_window.isOpen()) {
//        sf::Event err_event;
//        while (err_window.pollEvent(err_event)) {
//            if (err_event.type == sf::Event::Closed)
//                err_window.close();
//            quit_game();
//        }
//    }
//}