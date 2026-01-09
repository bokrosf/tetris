#include <string>
#include <asset.h>
#include <configuration.h>
#include <display.h>
#include <event.h>
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
        unsigned int score;
        unsigned int level;
        unsigned int line;
    };

    enum class movement
    {
        left,
        right,
        down,
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
        unsigned int center = state.grid.width / 2;
        unsigned int offset = (state.current.piece.width / 2) + (state.current.piece.width % 2);
        state.current.column = center - offset;
        copy(piece_templates[random_piece()], state.next);
    }

    bool collides(unsigned int area_row, unsigned int area_column)
    {
        for (unsigned int row = 0; row < state.current.piece.height; ++row)
        {
            for (unsigned int column = 0; column < state.current.piece.width; ++column)
            {
                if (state.current.piece.parts[row][column]
                    && state.grid.parts[area_row + row][area_column + column])
                {
                    return true;
                }
            }
        }

        return false;
    }

    void move(movement direction)
    {
        if (direction == movement::left
            && state.current.column > 0
            && !collides(state.current.row, state.current.column - 1))
        {
            --state.current.column;
        }
        else if (direction == movement::right
            && state.current.column < state.grid.width - state.current.piece.width
            && !collides(state.current.row, state.current.column + 1))
        {
            ++state.current.column;
        }
        else if (direction == movement::down
            && state.current.row > 0
            && !collides(state.current.row - 1, state.current.column))
        {
            --state.current.row;
        }
    }

    void drop()
    {
        unsigned int row = state.current.row;

        while (row > 0 && !collides(row - 1, state.current.column))
        {
            --row;
        }

        state.current.row = row;
    }

    void commit()
    {
        for (unsigned int row = 0; row < state.current.piece.height; ++row)
        {
            for (unsigned int column = 0; column < state.current.piece.width; ++column)
            {
                if (state.current.piece.parts[row][column])
                {
                    unsigned int grid_row = state.current.row + row;
                    unsigned int grid_column = state.current.column + column;
                    state.grid.parts[grid_row][grid_column] = state.current.piece.parts[row][column];
                }
            }
        }
    }

    void handle_input()
    {
        if (event::key_down(SDLK_LEFT))
        {
            move(movement::left);
        }

        if (event::key_down(SDLK_RIGHT))
        {
            move(movement::right);
        }

        if (event::key_down(SDLK_DOWN))
        {
            drop();
        }

        if (event::key_down(SDLK_SPACE))
        {
            commit();
            spawn_piece();
        }
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
            .score = 0,
            .level = 0,
            .line = 0,
        };

        state.grid.parts = new char *[state.grid.height];
        state.grid.parts[0] = new char[state.grid.height * state.grid.width];

        for (unsigned int row = 1; row < state.grid.height; ++row)
        {
            state.grid.parts[row] = state.grid.parts[0] + (row * state.grid.width);
        }

        clear_parts(state.grid.parts[0], state.grid.width, state.grid.height);
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
                .font = assets.font,
                .font_size = 4.0,
                .color = {.r = 1.0, .g = 1.0, .b = 1.0, .a = 1.0},
                .position = {.x = display::scaled(1400.0), .y = display::scaled(590.016)},
            },
            .piece
            {
                .width = static_cast<float>(display::mode.height) / state.grid.height,
                .texture = assets.piece_types,
            },
            .grid
            {
                .area
                {
                    .y = static_cast<float>(display::mode.height),
                },
                .color = {0xFF, 0xFF, 0xFF, 0xFF},
            },
            .next
            {
                .x = display::scaled(1450.0),
                .y = display::scaled(900.0),
            },
        };

        view.grid.area.w = state.grid.width * view.piece.width;
        view.grid.area.h = -(state.grid.height * view.piece.width);
        view.grid.area.x = (display::mode.width * 0.5F) - (0.5F * view.grid.area.w);
        view.left_wall =
        {
            .area =
            {
                .x = view.grid.area.x - view.piece.width,
                .y = 0,
                .w = view.piece.width,
                .h = static_cast<float>(display::mode.height)
            },
            .texture = assets.wall,
        };
        view.right_wall = view.left_wall;
        view.right_wall.area.x = view.grid.area.x + (view.grid.area.w);
    }

    void update_view()
    {
        view.current.x = view.grid.area.x + (state.current.column * view.piece.width);
        view.current.y = view.grid.area.y - (state.current.row * view.piece.width);
        view.score_value.text = std::to_string(state.score);
        view.level_value.text = std::to_string(state.level);
        view.lines_value.text = std::to_string(state.line);
    }

    void render_view()
    {
        render::begin_frame();
        render::draw(view.score_description);
        render::draw(view.score_value);
        render::draw(view.level_description);
        render::draw(view.level_value);
        render::draw(view.lines_description);
        render::draw(view.lines_value);
        render::draw_quad(view.grid.area, view.grid.color);

        render::draw(
            render::piece_grid
            {
                .width = part_dimension,
                .height = part_dimension,
                .position = view.current,
                .piece = view.piece,
                .parts = state.current.piece.parts[0],
            });

        render::draw(
            render::piece_grid
            {
                .width = part_dimension,
                .height = part_dimension,
                .position = view.next,
                .piece = view.piece,
                .parts = state.next.parts[0],
            });

        render::draw(
            render::piece_grid
            {
                .width = state.grid.width,
                .height = state.grid.height,
                .position = reinterpret_cast<SDL_FPoint &>(view.grid.area),
                .piece = view.piece,
                .parts = state.grid.parts[0],
            });

        render::draw_texture(assets.wall, view.left_wall.area);
        render::draw_texture(assets.wall, view.right_wall.area);

        render::end_frame();
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
        handle_input();
        // TODO: Update game state.
        update_view();
        render_view();
    }

    void shutdown()
    {
        delete[] state.grid.parts[0];
        delete[] state.grid.parts;
    }
}

