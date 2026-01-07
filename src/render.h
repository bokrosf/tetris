#ifndef TETRIS_RENDER_H
#define TETRIS_RENDER_H

#include <SDL3/SDL.h>
#include <asset.h>
#include <game.h>
#include <ui.h>

namespace render
{
    struct piece_grid
    {
        game::dimension width;
        game::dimension height;
        SDL_FPoint &position;
        ui::piece_grid &arguments;
        const char *parts;
    };

    void init();
    void shutdown();
    void begin_frame();
    void end_frame();
    void draw(const piece_grid &grid);
    void draw(const ui::label &label);
    void draw_texture(asset::id_type id, const SDL_FRect &area);
    SDL_Texture *load_texture(SDL_Surface *surface);
}

#endif
