#ifndef TETRIS_SETTINGS_H
#define TETRIS_SETTINGS_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <SDL3/SDL.h>

struct key_binding
{
    SDL_Keycode keyboard;
    SDL_GamepadButton controller;
};

struct settings_state
{
    bool vsync;
    std::unordered_map<std::string, key_binding> controls;
};

extern settings_state settings;

void load_settings(const std::filesystem::path &path);

#endif
