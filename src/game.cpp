#include <algorithm>
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
    };

    const int part_dimension = 4;
    const unsigned int rotation_count = 4;

    struct rotation_offset
    {
        int row;
        int column;
    };

    struct tetromino
    {
        int width;
        int height;
        char parts[part_dimension][part_dimension];
    };

    struct piece_grid
    {
        int width;
        int height;
        char **parts;
    };

    struct moving_piece
    {
        tetromino piece;
        int row;
        int column;
        unsigned int rotation;
    };

    struct game_state
    {
        moving_piece current;
        tetromino next;
        piece_grid grid;
        int score;
        int level;
        int line;
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

    const rotation_offset rotation_offsets[7][rotation_count] =
    {
        {{1, -1}, {-1, 1}, {1, -1}, {-1, 1}},
        {{0, -1}, {0, 0}, {1, 0}, {-1, 1}},
        {{0, -1}, {0, 0}, {1, 0}, {-1, 1}},
        {{0, -1}, {0, 0}, {1, 0}, {-1, 1}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
        {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    };

    void copy(const tetromino &from, tetromino &to)
    {
        to.width = from.width;
        to.height = from.height;

        for (int row = 0; row < part_dimension; ++row)
        {
            for (int column = 0; column < part_dimension; ++column)
            {
                to.parts[row][column] = from.parts[row][column];
            }
        }
    }

    void clear_parts(char *parts, int width, int height)
    {
        for (int row = 0; row < height; ++row)
        {
            for (int column = 0; column < width; ++column)
            {
                parts[(row * width) + column] = 0;
            }
        }
    }

    int random_piece()
    {
        return 1;
    }

    void spawn_piece()
    {
        copy(state.next, state.current.piece);
        state.current.row = state.grid.height - (state.current.piece.height + 1);
        int center = state.grid.width / 2;
        int offset = (state.current.piece.width / 2) + (state.current.piece.width % 2);
        state.current.column = center - offset;
        state.current.rotation = 0;
        copy(piece_templates[random_piece()], state.next);
    }

    bool collides(int area_row, int area_column)
    {
        for (int row = 0; row < state.current.piece.height; ++row)
        {
            for (int column = 0; column < state.current.piece.width; ++column)
            {
                int grid_row = std::clamp(area_row + row, 0, state.grid.height - 1);
                int grid_column = std::clamp(area_column + column, 0, state.grid.width - 1);

                if (state.current.piece.parts[row][column]
                    && state.grid.parts[grid_row][grid_column])
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool collides()
    {
        return collides(state.current.row, state.current.column);
    }

    void move(movement direction)
    {
        if (direction == movement::left
            && !collides(state.current.row, state.current.column - 1))
        {
            --state.current.column;
        }
        else if (direction == movement::right
            && !collides(state.current.row, state.current.column + 1))
        {
            ++state.current.column;
        }
        else if (direction == movement::down
            && !collides(state.current.row - 1, state.current.column))
        {
            --state.current.row;
        }
    }

    void rotate_left()
    {
        tetromino &t = state.current.piece;
        tetromino other{.width = t.height, .height = t.width};
        clear_parts(other.parts[0], part_dimension, part_dimension);
        unsigned int type = 0;

        for (int row = 0; row < t.height; ++row)
        {
            for (int column = 0; column < t.width; ++column)
            {
                type |= t.parts[row][column];
                other.parts[column][other.width - 1 - row] = t.parts[row][column];
            }
        }

        copy(other, t);
        --type;
        const rotation_offset &offset = rotation_offsets[type][state.current.rotation];
        state.current.row -= offset.row;
        state.current.column -= offset.column;
        --state.current.rotation;
        state.current.rotation %= rotation_count;
    }

    void rotate_right()
    {
        tetromino &t = state.current.piece;
        tetromino other{.width = t.height, .height = t.width};
        clear_parts(other.parts[0], part_dimension, part_dimension);
        unsigned int type = 0;

        for (int row = 0; row < t.height; ++row)
        {
            for (int column = 0; column < t.width; ++column)
            {
                type |= t.parts[row][column];
                other.parts[other.height - 1 - column][row] = t.parts[row][column];
            }
        }

        copy(other, t);
        --type;
        ++state.current.rotation;
        state.current.rotation %= rotation_count;
        const rotation_offset &offset = rotation_offsets[type][state.current.rotation];
        state.current.row += offset.row;
        state.current.column += offset.column;
    }

    void drop()
    {
        int row = state.current.row;

        while (!collides(row - 1, state.current.column))
        {
            --row;
        }

        state.current.row = row;
    }

    void commit()
    {
        for (int row = 0; row < state.current.piece.height; ++row)
        {
            for (int column = 0; column < state.current.piece.width; ++column)
            {
                if (state.current.piece.parts[row][column])
                {
                    int grid_row = state.current.row + row;
                    int grid_column = state.current.column + column;
                    state.grid.parts[grid_row][grid_column] = state.current.piece.parts[row][column];
                }
            }
        }
    }

    void clear_complete_lines()
    {
        const int highest_row = state.current.row + state.current.piece.height - 1;
        int count = 0;
        bool complete = true;

        for (int row = state.current.row; complete && row <= highest_row; ++row)
        {
            int column = 1;

            while (complete && column < state.grid.width - 1)
            {
                complete &= state.grid.parts[row][column] != 0;
                ++column;
            }

            complete &= column == state.grid.width - 1;
            count += complete;
        }

        if (count < 1)
        {
            return;
        }

        for (int row = state.current.row + count; row < state.grid.height; ++row)
        {
            for (int column = 1; column < state.grid.width - 1; ++column)
            {
                state.grid.parts[row - count][column] = state.grid.parts[row][column];
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

        if (event::key_down(SDLK_A))
        {
            rotate_left();

            if (collides())
            {
                rotate_right();
            }
        }

        if (event::key_down(SDLK_D))
        {
            rotate_right();

            if (collides())
            {
                rotate_left();
            }
        }

        if (event::key_down(SDLK_SPACE))
        {
            commit();
            clear_complete_lines();
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

        for (int row = 1; row < state.grid.height; ++row)
        {
            state.grid.parts[row] = state.grid.parts[0] + (row * state.grid.width);
        }

        clear_parts(state.grid.parts[0], state.grid.width, state.grid.height);

        const char wall = 8;
        for (int row = 0; row < state.grid.height; ++row)
        {
            state.grid.parts[row][0] = wall;
            state.grid.parts[row][state.grid.width - 1] = wall;
        }

        for (int column = 1; column < state.grid.width - 1; ++column)
        {
            state.grid.parts[0][column] = wall;
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
        view.grid.area.y = static_cast<float>(state.grid.height * view.piece.width);
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

