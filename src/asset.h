#ifndef TETRIS_ASSET_H
#define TETRIS_ASSET_H

#include <string>
#include <font.h>

namespace asset
{
    using id_type = unsigned int;

    id_type load_font(const std::string &path);
    void unload_all();
    ui::font &font(id_type id);
}

#endif
