#ifndef TETRIS_CONFIGURATION_H
#define TETRIS_CONFIGURATION_H

#include <string>
#include <nlohmann/json.hpp>

struct game_configuration
{
    unsigned int grid_width;
    unsigned int grid_height;
};

struct configuration
{
    std::string font_path;
    std::string piece_types_texture_path;
    std::string wall_texture_path;
    std::string settings_path;
    game_configuration gameplay;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(game_configuration, grid_width, grid_height)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( \
    configuration, \
    font_path, \
    piece_types_texture_path, \
    wall_texture_path, \
    settings_path, \
    gameplay) \

extern configuration config;

void load_configuration(const std::string &path);

#endif
