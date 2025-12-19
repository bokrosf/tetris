#ifndef TETRIS_ASSET_H
#define TETRIS_ASSET_H

#include <string>
#include <unordered_map>
#include <vector>
#include <SDL3/SDL.h>

namespace asset
{
    struct font
    {
        float width;
        std::unordered_map<char, std::vector<SDL_Vertex>> vertices;
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
    }
}

#endif
