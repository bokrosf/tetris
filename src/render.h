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
        int width;
        int height;
        SDL_FPoint &position;
        ui::piece_config &piece;
        const char *parts;
    };

    void init();
    void shutdown();
    void begin_frame();
    void end_frame();
    void draw(const piece_grid &grid);
    void draw(const ui::label &label);
    void draw_quad(const SDL_FRect &area, const SDL_Color &color);
    void draw_quad(const SDL_FRect &area, const SDL_FColor &color);
    SDL_Texture *load_texture(SDL_Surface *surface);
}

#endif
