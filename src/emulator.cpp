#include "memory.h"
#include "cpu.h"
#include "gpu.h"
#include "emulator.h"

Emulator::Emulator() {
    cpu = std::make_unique<CPU>();
    mem = std::make_unique<Memory>();
    gpu = std::make_unique<GPU>();

    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Chip-8 emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 640, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << SDL_GetError() << std::endl;
    }

    if (TTF_Init() == -1) {
        std::cout << TTF_GetError() << std::endl;
    }

    font_ = TTF_OpenFont("assets/font.ttf", 60);
    if (!font_) {
        std::cout << TTF_GetError() << std::endl;
    }

    keycap_font_ = TTF_OpenFont("assets/glyphs.otf", 60);
    if (!keycap_font_) {
        std::cout << TTF_GetError() << std::endl;
    }

    int result = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    if (result != 0) {
        std::cout << Mix_GetError() << std::endl;
    }

    sfx = Mix_LoadWAV("assets/beep-1200.wav");
    if (!sfx) {
        std::cout << Mix_GetError() << std::endl;
    }

    Mix_VolumeChunk(sfx, 30);

    // fills the game_entries_ vector with all the roms (std::filesystem::directory_entry's)
    if (!std::filesystem::is_empty(games_path_)) {
        for (auto const& rom : std::filesystem::directory_iterator{ games_path_ })
            if (rom.path().extension() == ".ch8") {
                games_entries_.push_back(rom);
            }
    }

    list_games();
}

Emulator::~Emulator() {}

uint16_t Emulator::decode_key_pressed() {
    const uint8_t* state =  SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_0]) {
        return 0x0;
    }
    else if (state[SDL_SCANCODE_1]) {
        return 0x1;
    }
    else if (state[SDL_SCANCODE_2]) {
        return 0x2;
    }
    else if (state[SDL_SCANCODE_3]) {
        return 0x3;
    }
    else if (state[SDL_SCANCODE_4]) {
        return 0x4;
    }
    else if (state[SDL_SCANCODE_5]) {
        return 0x5;
    }
    else if (state[SDL_SCANCODE_6]) {
        return 0x6;
    }
    else if (state[SDL_SCANCODE_7]) {
        return 0x7;
    }
    else if (state[SDL_SCANCODE_8]) {
        return 0x8;
    }
    else if (state[SDL_SCANCODE_9]) {
        return 0x9;
    }
    else if (state[SDL_SCANCODE_A]) {
        return 0xA;
    }
    else if (state[SDL_SCANCODE_B]) {
        return 0xB;
    }
    else if (state[SDL_SCANCODE_C]) {
        return 0xC;
    }
    else if (state[SDL_SCANCODE_D]) {
        return 0xD;
    }
    else if (state[SDL_SCANCODE_E]) {
        return 0xE;
    }
    else if (state[SDL_SCANCODE_F]) {
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
    TTF_SetFontSize(font_, 60);

    SDL_Surface* logo_surface = TTF_RenderText_Solid(font_, "Chip-8 Emulator", SDL_Color{ 255, 255, 255 });
    if (!logo_surface) {
        std::cout << TTF_GetError() << std::endl;
    }

    SDL_Texture* logo_texture = SDL_CreateTextureFromSurface(renderer, logo_surface);
    if (!logo_texture) {
        std::cout << SDL_GetError() << std::endl;
    }
    SDL_FreeSurface(logo_surface);

    SDL_Rect logo_rect = { 20, 20, 0, 0 };
    SDL_QueryTexture(logo_texture, NULL, NULL, &logo_rect.w, &logo_rect.h);

    SDL_RenderCopy(renderer, logo_texture, NULL, &logo_rect);

    SDL_DestroyTexture(logo_texture);
}

void Emulator::draw_single_control_legend_(const char* keycap_text, const char* legend_text, int pos_x, int pos_y) {
    SDL_Surface* keycap_surface = TTF_RenderText_Solid(keycap_font_, keycap_text, SDL_Color{ 255, 255, 255 });
    if (!keycap_surface) {
        std::cout << TTF_GetError() << std::endl;
    }

    SDL_Texture* keycap_texture = SDL_CreateTextureFromSurface(renderer, keycap_surface);
    if (!keycap_texture) {
        std::cout << SDL_GetError() << std::endl;
    }
    SDL_FreeSurface(keycap_surface);

    SDL_Rect keycap_rect = { pos_x, pos_y, 0, 0 };
    SDL_QueryTexture(keycap_texture, NULL, NULL, &keycap_rect.w, &keycap_rect.h);
    SDL_RenderCopy(renderer, keycap_texture, NULL, &keycap_rect);
    SDL_DestroyTexture(keycap_texture);

    TTF_SetFontSize(font_, 22);
    SDL_Surface* legend_surface = TTF_RenderText_Solid(font_, legend_text, SDL_Color{ 255, 255, 255 });
    if (!legend_surface) {
        std::cout << TTF_GetError() << std::endl;
    }

    SDL_Texture* legend_texture = SDL_CreateTextureFromSurface(renderer, legend_surface);
    if (!legend_texture) {
        std::cout << SDL_GetError() << std::endl;
    }
    SDL_FreeSurface(legend_surface);

    SDL_Rect legend_rect = { (keycap_rect.x + keycap_rect.w + 20), (keycap_rect.y + keycap_rect.h / 4), 0, 0 };
    SDL_QueryTexture(legend_texture, NULL, NULL, &legend_rect.w, &legend_rect.h);
    SDL_RenderCopy(renderer, legend_texture, NULL, &legend_rect);
    SDL_DestroyTexture(legend_texture);
}

void Emulator::draw_controls_legend_() {
    draw_single_control_legend_("W", "GO UP", 1000, 20);
    draw_single_control_legend_("S", "GO DOWN", 1000, 80);
    draw_single_control_legend_("R", "RESET GAME", 1000, 140);
    draw_single_control_legend_("a", "SELECT GAME", 1000, 200);
    draw_single_control_legend_("m", "QUIT TO MENU", 1000, 260);
}

void Emulator::list_games() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    draw_logo_();
    draw_controls_legend_();

    TTF_SetFontSize(font_, 28);
    uint16_t line_pos_x = 20;
    uint16_t line_pos_y = 110;

    SDL_Surface* game_name_surface;
    SDL_Color white = SDL_Color{ 255, 255, 255 };
    SDL_Color cyan = SDL_Color{ 0, 255, 255 };

    if (games_entries_.empty()) {
        game_name_surface = TTF_RenderUTF8_Solid(font_, "No roms available", white);

        SDL_Texture* game_name_texture = SDL_CreateTextureFromSurface(renderer, game_name_surface);
        if (!game_name_texture) {
            std::cout << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(game_name_surface);

        SDL_Rect game_name_rect = { line_pos_x, line_pos_y, 0, 0 };
        SDL_QueryTexture(game_name_texture, NULL, NULL, &game_name_rect.w, &game_name_rect.h);

        SDL_RenderCopy(renderer, game_name_texture, NULL, &game_name_rect);
        SDL_RenderPresent(renderer);

        SDL_DestroyTexture(game_name_texture);
        return;
    }

    uint16_t page_start = (cursor_position / GAMES_PER_PAGE_) * GAMES_PER_PAGE_;
    for (uint16_t i = page_start; i < (page_start + GAMES_PER_PAGE_); i++) {
        if (i >= num_games_installed()) {
            break;
        }

        if (i == cursor_position) {
            game_name_surface = TTF_RenderUTF8_Solid(font_, games_entries_[i].path().stem().string().c_str(), cyan);
        }
        else {
            game_name_surface = TTF_RenderUTF8_Solid(font_, games_entries_[i].path().stem().string().c_str(), white);
        }
        if (!game_name_surface) {
            std::cout << TTF_GetError() << std::endl;
        }

        SDL_Texture* game_name_texture = SDL_CreateTextureFromSurface(renderer, game_name_surface);
        if (!game_name_texture) {
            std::cout << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(game_name_surface);

        SDL_Rect game_name_rect = { line_pos_x, line_pos_y, 0, 0 };
        SDL_QueryTexture(game_name_texture, NULL, NULL, &game_name_rect.w, &game_name_rect.h);

        SDL_RenderCopy(renderer, game_name_texture, NULL, &game_name_rect);

        SDL_DestroyTexture(game_name_texture);

        line_pos_y += 50;
    }
    SDL_RenderPresent(renderer);
}

std::filesystem::directory_entry Emulator::game_selected() {
    return games_entries_[cursor_position];
}

void Emulator::quit_game() {
    Mix_HaltChannel(-1);
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
