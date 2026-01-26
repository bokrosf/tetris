#ifndef TETRIS_ASSET_H
#define TETRIS_ASSET_H

#include <filesystem>
#include <SDL3/SDL.h>
#include <font.h>

namespace asset
{
    using id_type = unsigned int;

    id_type load_font(const std::filesystem::path &path);
    id_type load_texture(const std::filesystem::path &path);
    void unload_all();
    ui::font &font(id_type id);
    SDL_Texture &texture(id_type id);
}

#endif
