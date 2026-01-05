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

    struct piece_grid
    {
        SDL_FPoint position;
        float width;
        float separator;
    };

    struct game_layout
    {
        label score_description;
        label score_value;
        label level_description;
        label level_value;
        label lines_description;
        label lines_value;
        piece_grid grid;
        piece_grid current;
        piece_grid next;
    };
}

#endif
