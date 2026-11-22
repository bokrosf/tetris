#ifndef TETRIS_CONFIGURATION_H
#define TETRIS_CONFIGURATION_H

#include <array>
#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>

struct game_configuration
{
    int level_line_requirement;
    std::array<int, 4> line_scores;
    unsigned int strafe_interval;
    std::array<int, 21> fall_intervals;
    int soft_drop_cells;
    unsigned int line_animation_duration_ms;
    unsigned int field_fill_animation_duration_ms;
    unsigned int field_clear_animation_duration_ms;
};

struct aspect_ratio_configuration
{
    int width;
    int height;
};

struct ui_configuration
{
    int design_height;
    float font_height;
    aspect_ratio_configuration aspect_ratio;
};

struct configuration
{
    std::filesystem::path font_path;
    std::filesystem::path piece_types_texture_path;
    std::filesystem::path settings_path;
    std::filesystem::path views_path;
    game_configuration gameplay;
    ui_configuration ui;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    game_configuration, \
    level_line_requirement, \
    line_scores, \
    strafe_interval, \
    fall_intervals, \
    soft_drop_cells, \
    line_animation_duration_ms, \
    field_fill_animation_duration_ms, \
    field_clear_animation_duration_ms)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    aspect_ratio_configuration, \
    width, \
    height)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
    ui_configuration, \
    design_height, \
    font_height, \
    aspect_ratio)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( \
    configuration, \
    font_path, \
    piece_types_texture_path, \
    settings_path, \
    views_path, \
    gameplay, \
    ui)

extern configuration config;

void load_configuration(const std::filesystem::path &path);

#endif
