#ifndef TETRIS_RENDER_H
#define TETRIS_RENDER_H

#include <SDL3/SDL.h>
#include <ui.h>

namespace render
{
    struct piece_grid
    {
        bool visible;
        int width;
        int height;
        SDL_FPoint &position;
        ui::piece_config &piece;
        const char *parts;
    };

    void init();
    void shutdown();
    void adapt_to_window();
    void toggle_fullscreen();
    void begin_frame();
    void end_frame();
    void draw(const piece_grid &grid);
    void draw(const ui::label &label);
    void draw_quad(const SDL_FRect &area, const SDL_FColor &color);
    SDL_Texture *load_texture(SDL_Surface *surface);
    void unload_texture(SDL_Texture *texture);
}

#endif
