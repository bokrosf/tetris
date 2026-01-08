#include <asset.h>
#include <configuration.h>
#include <display.h>
#include <game.h>
#include <render.h>
#include <ui.h>

using namespace game;

namespace
{
    struct asset_keys
    {
        asset::id_type font;
        asset::id_type piece_types;
        asset::id_type wall;
    };

    const unsigned int part_dimension = 4;

    struct tetromino
    {
        unsigned int width;
        unsigned int height;
        char parts[part_dimension][part_dimension];
    };

    struct piece_grid
    {
        unsigned int width;
        unsigned int height;
        char **parts;
    };

    struct moving_piece
    {
        tetromino piece;
        unsigned int row;
        unsigned int column;
    };

    struct game_state
    {
        moving_piece current;
        tetromino next;
        piece_grid grid;
    };

    game_state state;
    ui::game_layout view;
    asset_keys assets;

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

        for (unsigned int row = 0; row < part_dimension; ++row)
        {
            for (unsigned int column = 0; column < part_dimension; ++column)
            {
                to.parts[row][column] = from.parts[row][column];
            }
        }
    }

    void clear_parts(char *parts, unsigned int width, unsigned int height)
    {
        for (unsigned int row = 0; row < height; ++row)
        {
            for (unsigned int column = 0; column < width; ++column)
            {
                parts[(row * width) + column] = 0;
            }
        }
    }

    void rotate(tetromino &t)
    {
        tetromino other{.width = t.height, .height = t.width};
        clear_parts(other.parts[0], part_dimension, part_dimension);

        for (unsigned int row = 0; row < t.height; ++row)
        {
            for (unsigned int column = 0; column < t.width; ++column)
            {
                other.parts[column][t.height - 1 - row] = t.parts[row][column];
                t.parts[row][column] = 0;
            }
        }

        copy(other, t);
    }

    unsigned int random_piece()
    {
        return 0;
    }

    void spawn_piece()
    {
        copy(state.next, state.current.piece);
        state.current.row = state.grid.height - (state.current.piece.height + 1);
        unsigned int center = (state.grid.width - 1) / 2;
        unsigned int offset = (state.current.piece.width / 2) - 1;
        state.current.column = center - offset;
        copy(piece_templates[random_piece()], state.next);
    }

    void init_state()
    {
        state = game_state
        {
            .grid =
            {
                .width = config.gameplay.grid_width,
                .height = config.gameplay.grid_height,
            },
        };

        state.grid.parts = new char *[state.grid.height];
        state.grid.parts[0] = new char[state.grid.height * state.grid.width];

        for (unsigned int row = 1; row < state.grid.height; ++row)
        {
            state.grid.parts[row] = state.grid.parts[0] + (row * state.grid.width);
        }

        clear_parts(state.grid.parts[0], state.grid.width, state.grid.height);

        for (unsigned int row = 0; row < state.grid.height; ++row)
        {
            for (unsigned int column = 0; column < state.grid.width; ++column)
            {
                state.grid.parts[row][column] = 1;
            }
        }

        copy(piece_templates[random_piece()], state.next);
        spawn_piece();
    }

    void init_view()
    {
        assets =
        {
            .font = asset::load_font(config.font_path),
            .piece_types = asset::load_texture(config.piece_types_texture_path),
            .wall = asset::load_texture(config.wall_texture_path),
        };

        view = ui::game_layout
        {
            .score_description = ui::label
            {
                .text = "SCORE",
                .font = assets.font,
                .font_size = 4.0,
                .color = {.r = 1.0, .g = 1.0, .b = 1.0, .a = 1.0},
                .position = {.x = display::scaled(1400.0), .y = display::scaled(80.0)},
            },
            .score_value
            {
                .text = "9999999",
                .font = assets.font,
                .font_size = 4.0,
                .color = {.r = 1.0, .g = 1.0, .b = 1.0, .a = 1.0},
                .position = {.x = display::scaled(1400.0), .y = display::scaled(150.016)},
            },
            .level_description
            {
                .text = "LEVEL",
                .font = assets.font,
                .font_size = 4.0,
                .color = {.r = 1.0, .g = 1.0, .b = 1.0, .a = 1.0},
                .position = {.x = display::scaled(1400.0), .y = display::scaled(300.0)},
            },
            .level_value
            {
                .text = "9999999",
                .font = assets.font,
                .font_size = 4.0,
                .color = {.r = 1.0, .g = 1.0, .b = 1.0, .a = 1.0},
                .position = {.x = display::scaled(1400.0), .y = display::scaled(370.016)},
            },
            .lines_description
            {
                .text = "LINES",
                .font = assets.font,
                .font_size = 4.0,
                .color = {.r = 1.0, .g = 1.0, .b = 1.0, .a = 1.0},
                .position = {.x = display::scaled(1400.0), .y = display::scaled(520.0)},
            },
            .lines_value
            {
                .text = "9999999",
                .font = assets.font,
                .font_size = 4.0,
                .color = {.r = 1.0, .g = 1.0, .b = 1.0, .a = 1.0},
                .position = {.x = display::scaled(1400.0), .y = display::scaled(590.016)},
            },
            .piece_config
            {
                .width = static_cast<float>(display::mode.height) / state.grid.height,
                .texture = assets.piece_types,
            },
            .grid
            {
                .y = static_cast<float>(display::mode.height),
            },
            .current
            {
                .x = 100,
                .y = 300,
            },
            .next
            {
                .x = display::scaled(1450.0),
                .y = display::scaled(900.0),
            },
        };

        view.grid.x = (display::mode.width * 0.5) - (0.5 * state.grid.width * view.piece_config.width);
        view.left_wall =
        {
            .area =
            {
                .x = view.grid.x - view.piece_config.width,
                .y = 0,
                .w = view.piece_config.width,
                .h = static_cast<float>(display::mode.height)
            },
            .texture = assets.wall,
        };
        view.right_wall = view.left_wall;
        view.right_wall.area.x = view.grid.x + (state.grid.width * view.piece_config.width);
    }

    void update_view()
    {
        view.current.x = view.grid.x + (state.current.column * view.piece_config.width);
        view.current.y = view.grid.y - (state.current.row * view.piece_config.width);
    }
}

namespace game
{
    void init()
    {
        init_state();
        init_view();
    }

    void run()
    {
        // TODO: Update game state.
        // TODO: Update view state.
        update_view();
        render::begin_frame();
        render::draw(view.score_description);
        render::draw(view.score_value);
        render::draw(view.level_description);
        render::draw(view.level_value);
        render::draw(view.lines_description);
        render::draw(view.lines_value);

        render::draw(
            render::piece_grid
            {
                .width = part_dimension,
                .height = part_dimension,
                .position = view.current,
                .arguments = view.piece_config,
                .parts = state.current.piece.parts[0],
            });

        render::draw(
            render::piece_grid
            {
                .width = part_dimension,
                .height = part_dimension,
                .position = view.next,
                .arguments = view.piece_config,
                .parts = state.next.parts[0],
            });

        render::draw(
            render::piece_grid
            {
                .width = state.grid.width,
                .height = state.grid.height,
                .position = view.grid,
                .arguments = view.piece_config,
                .parts = state.grid.parts[0],
            });

        render::draw_texture(assets.wall, view.left_wall.area);
        render::draw_texture(assets.wall, view.right_wall.area);
        render::end_frame();
    }

    void shutdown()
    {
        delete[] state.grid.parts[0];
        delete[] state.grid.parts;
    }
}

