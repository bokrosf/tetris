#include <asset.h>
#include <configuration.h>
#include <game.h>
#include <render.h>
#include <ui.h>

using namespace game;

namespace
{
    game_state state;
    ui::game_layout view;

    const tetromino piece_templates[7] =
    {
        tetromino
        {
            .width = 4,
            .height = 1,
            .parts =
            {
                {1, 1, 1, 1},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }
        },
        tetromino
        {
            .width = 3,
            .height = 2,
            .parts =
            {
                {2, 2, 2, 0},
                {0, 2, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }
        },
        tetromino
        {
            .width = 3,
            .height = 2,
            .parts =
            {
                {3, 3, 3, 0},
                {0, 0, 3, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }
        },
        tetromino
        {
            .width = 3,
            .height = 2,
            .parts =
            {
                {4, 4, 4, 0},
                {4, 0, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }
        },
        tetromino
        {
            .width = 2,
            .height = 2,
            .parts =
            {
                {5, 5, 0, 0},
                {5, 5, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }
        },
        tetromino
        {
            .width = 3,
            .height = 2,
            .parts =
            {
                {6, 6, 0, 0},
                {0, 6, 6, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }
        },
        tetromino
        {
            .width = 3,
            .height = 2,
            .parts =
            {
                {0, 7, 7, 0},
                {7, 7, 0, 0},
                {0, 0, 0, 0},
                {0, 0, 0, 0},
            }
        },
    };

    void copy(const tetromino &from, tetromino &to)
    {
        to.width = from.width;
        to.height = from.height;

        for (dimension row = 0; row < from.height; ++row)
        {
            for (dimension column = 0; column < from.width; ++column)
            {
                to.parts[row][column] = from.parts[row][column];
            }
        }
    }

    void clear_parts(char **parts, dimension width, dimension height)
    {
        for (dimension row = 0; row < height; ++row)
        {
            for (dimension column = 0; column < width; ++column)
            {
                parts[row][column] = 0;
            }
        }
    }

    void rotate(tetromino &t)
    {
        tetromino other;
        clear_parts(reinterpret_cast<char **>(other.parts), part_dimension, part_dimension);

        for (dimension row = 0; row < t.height; ++row)
        {
            for (dimension column = 0; column < t.width; ++column)
            {
                other.parts[column][t.height - 1 - row] = t.parts[row][column];
                t.parts[row][column] = ' ';
            }
        }

        std::swap(t.width, t.height);

        for (dimension row = 0; row < t.height; ++row)
        {
            for (dimension column = 0; column < t.width; ++column)
            {
                t.parts[row][column] = other.parts[row][column];
            }
        }
    }
}

namespace game
{
    void init()
    {
        asset::id_type font = asset::load_font(config.font_path);

        state = game::game_state();
        view = ui::game_layout
        {
            .score = ui::label
            {
                .text = "Tetris",
                .font = font,
                .font_size = 1,
                .color = {.r = 0.0, .g = 0.0, .b = 1.0, .a = 1.0},
                .position = {.x = 0.0, .y = 0.0}
            }
        };
    }

    void run()
    {
        // TODO: Update game state.
        // TODO: Update view state.
        render::draw_frame(state, view);
    }
}

