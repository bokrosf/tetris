#ifndef TETRIS_CONFIGURATION_H
#define TETRIS_CONFIGURATION_H

#include <array>
#include <string>
#include <nlohmann/json.hpp>

struct scoring_configuration
{
    int max_level;
    std::array<int, 4> line_values;
};

struct game_configuration
{
    int grid_width;
    int grid_height;
    scoring_configuration scoring;
};

struct configuration
{
    std::string font_path;
    std::string piece_types_texture_path;
    std::string settings_path;
    game_configuration gameplay;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    scoring_configuration, \
    max_level, \
    line_values)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    game_configuration, \
    grid_width, \
    grid_height, \
    scoring)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( \
    configuration, \
    font_path, \
    piece_types_texture_path, \
    settings_path, \
    gameplay) \

extern configuration config;

void load_configuration(const std::string &path);

#endif
