#ifndef TETRIS_CONFIGURATION_H
#define TETRIS_CONFIGURATION_H

#include <array>
#include <string>
#include <nlohmann/json.hpp>

struct game_configuration
{
    int grid_width;
    int grid_height;
    int level_line_requirement;
    std::array<int, 4> line_scores;
    std::array<int, 21> fall_intervals;
    int soft_drop_cells;
};

struct configuration
{
    std::string font_path;
    std::string piece_types_texture_path;
    std::string settings_path;
    game_configuration gameplay;
};


NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    game_configuration, \
    grid_width, \
    grid_height, \
    level_line_requirement, \
    line_scores, \
    fall_intervals, \
    soft_drop_cells)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( \
    configuration, \
    font_path, \
    piece_types_texture_path, \
    settings_path, \
    gameplay) \

extern configuration config;

void load_configuration(const std::string &path);

#endif
