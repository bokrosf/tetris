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
                {0, 2, 0, 0},
                {2, 2, 2, 0},
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
                {0, 0, 3, 0},
                {3, 3, 3, 0},
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
                {4, 0, 0, 0},
                {4, 4, 4, 0},
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
                {0, 6, 6, 0},
                {6, 6, 0, 0},
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
                {7, 7, 0, 0},
                {0, 7, 7, 0},
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
                t.parts[row][column] = 0;
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

        state = game::game_state
        {
            .grid =
            {
                .width = config.gameplay.grid_width,
                .height = config.gameplay.grid_height,
            },
        };

        state.grid.parts = new char *[state.grid.height];
        state.grid.parts[0] = new char[state.grid.height * state.grid.width];

        for (dimension row = 1; row < state.grid.height; ++row)
        {
            state.grid.parts[row] = state.grid.parts[0] + (row * state.grid.width);
        }

        clear_parts(state.grid.parts, state.grid.width, state.grid.height);

        copy(piece_templates[0], state.current);
        copy(piece_templates[1], state.next);

        view = ui::game_layout
        {
            .score = ui::label
            {
                .text = "Tetris",
                .font = font,
                .font_size = 20.0,
                .color = {.r = 0.0, .g = 0.0, .b = 1.0, .a = 1.0},
                .position = {.x = 0.0, .y = 0.0}
            },
            .current = SDL_FPoint{.x = 100, .y = 300},
            .next = SDL_FPoint{.x = 100, .y = 700},
        };
    }

    void run()
    {
        // TODO: Update game state.
        // TODO: Update view state.
        render::draw_frame(state, view);
    }

    void shutdown()
    {
        delete[] state.grid.parts[0];
        delete[] state.grid.parts;
    }
}

