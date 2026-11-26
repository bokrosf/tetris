#ifndef TETRIS_SETTINGS_H
#define TETRIS_SETTINGS_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <SDL3/SDL.h>

struct crt_filter_settings
{
    bool enabled;
    float spacing;
    SDL_FColor color;
};

struct graphics_settings
{
    bool vsync;
    crt_filter_settings crt_filter;
};

struct key_binding
{
    SDL_Keycode keyboard;
    SDL_GamepadButton gamepad;
};

struct settings_state
{
    graphics_settings graphics;
    std::unordered_map<std::string, key_binding> controls;
};

extern settings_state settings;

void load_settings(const std::filesystem::path &path);

void init_settings();
void run_settings();

#endif
