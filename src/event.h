#ifndef TETRIS_EVENT_H
#define TETRIS_EVENT_H

#include <vector>
#include <SDL3/SDL.h>

namespace event
{
    void init();
    void shutdown();
    void read_all();
    void clear();
    bool occured(SDL_EventType type);
    const std::vector<SDL_Event> &queue();
    bool key_down(SDL_Keycode key);
    bool key_up(SDL_Keycode key);
    bool button_down(SDL_GamepadButton button);
    bool button_up(SDL_GamepadButton button);
}

#endif
