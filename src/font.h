#ifndef TETRIS_FONT_H
#define TETRIS_FONT_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct vertex
{
    float x;
    float y;
};

struct symbol
{
    char code;
    std::vector<vertex> vertices;
};

struct font
{
    float width;
    std::vector<symbol> symbols;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(vertex, x, y)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(symbol, code, vertices)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(font, width, symbols)

font load_font(const std::string &path);

#endif
