#ifndef TETRIS_UI_H
#define TETRIS_UI_H

#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>
#include <asset.h>
#include <serialization.h>

namespace ui
{
    struct label
    {
        bool visible;
        std::string text;
        asset::id_type font;
        float font_size;
        SDL_FColor color;
        SDL_FPoint position;
    };

    struct piece_config
    {
        float width;
        asset::id_type texture;
    };

    struct colored_area
    {
        SDL_FRect area;
        SDL_FColor color;
    };

    struct part_grid
    {
        bool visible;
        SDL_FPoint position;
        colored_area background;
        SDL_FColor complete_line_color;
    };

    struct piece
    {
        bool visible;
        SDL_FPoint position;
    };

    struct score_list
    {
        label item_template;
        std::vector<label> items;
        int spacing;
    };

    struct game_layout
    {
        label score_description;
        label score_value;
        label level_description;
        label level_value;
        label lines_description;
        label lines_value;
        piece_config piece;
        part_grid grid;
        ui::piece current;
        ui::piece next;
        label game_over;
        label top_score;
        score_list scores;
    };

    struct scores_layout
    {
        label title;
        score_list scores;
    };

    template<typename T>
    T load_view(const std::filesystem::path &path)
    {
        using json = nlohmann::ordered_json;

        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error(std::format("View file not found: {}", path.c_str()));
        }

        std::ifstream input(path);
        json view_json = json::parse(input);

        return view_json.get<T>();
    }

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        label, \
        visible, \
        text, \
        font, \
        font_size, \
        color, \
        position)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        piece_config, \
        width)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        colored_area, \
        area, \
        color)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        part_grid, \
        visible, \
        position, \
        background, \
        complete_line_color)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        piece, \
        visible, \
        position)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        score_list, \
        item_template, \
        spacing)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        game_layout, \
        score_description, \
        score_value, \
        level_description, \
        level_value, \
        lines_description, \
        lines_value, \
        piece, \
        grid, \
        current, \
        next, \
        game_over, \
        top_score, \
        scores)

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        scores_layout, \
        title, \
        scores)
}

#endif
