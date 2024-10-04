#include <iostream>
#include <memory>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Emulator {
public:
    Emulator();
    ~Emulator();

    std::unique_ptr<CPU> cpu;
    std::unique_ptr<Memory> mem;
    std::unique_ptr<GPU> gpu;
    std::unique_ptr<sf::RenderWindow> window; 
    std::unique_ptr<sf::Sound> sound;

    bool game_is_loaded = false;
    uint16_t instructions_ran = 0;
    uint16_t cursor_position = 0;

    uint16_t decode_key_pressed();
    uint16_t num_games_installed();
    void list_games();
    void quit_game();
    void reload_game();
    void reset_system();
    std::filesystem::directory_entry game_selected();

private:
    uint16_t GAMES_PER_PAGE_ = 10;

    sf::SoundBuffer buffer_;
    sf::Font font_;
    sf::Text text_;

    std::filesystem::path games_path_ = "./roms";
    std::vector<std::filesystem::directory_entry> games_entries_;

    void draw_logo_();
    void draw_control_glyphs_();

};