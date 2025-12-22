#ifndef TETRIS_UI_H
#define TETRIS_UI_H

#include <string>
#include <SDL3/SDL.h>
#include <asset.h>

namespace ui
{
    struct label
    {
        std::string text;
        asset::id_type font;
        float font_size;
        SDL_FColor color;
        SDL_FPoint position;
    };

    struct game_layout
    {
        label score;
        SDL_FPoint current;
        SDL_FPoint next;
    };
}

#endif
