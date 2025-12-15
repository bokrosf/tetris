#ifndef TETRIS_EVENT_H
#define TETRIS_EVENT_H

#include <SDL3/SDL.h>

namespace event
{
    void init();
    void shutdown();
    void read_all();
    bool occured(SDL_EventType type);
    bool key_down(SDL_Keycode key);
}

#endif
