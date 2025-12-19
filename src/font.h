#ifndef TETRIS_FONT_H
#define TETRIS_FONT_H

#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

namespace ui
{
    struct font
    {
        float width;
        std::unordered_map<char, std::vector<SDL_Vertex>> symbols;
    };

    namespace storage
    {
        struct font_vertex
        {
            float x;
            float y;
        };

        struct symbol
        {
            char code;
            std::vector<font_vertex> vertices;
        };

        struct font
        {
            float width;
            std::vector<symbol> symbols;
        };

        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(font_vertex, x, y)
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(symbol, code, vertices)
        NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(font, width, symbols)
    }
}

#endif
