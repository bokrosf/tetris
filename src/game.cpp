#include <algorithm>
#include <array>
#include <cmath>
#include <numbers>
#include <random>
#include <string>
#include <asset.h>
#include <configuration.h>
#include <game.h>
#include <input.h>
#include <navigation.h>
#include <render.h>
#include <scores.h>
#include <timer.h>
#include <ui.h>

using namespace game;

namespace
{
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

    struct drop_state
    {
        bool enabled;
        bool dropping;
        int rows;
    };

    struct line_completion
    {
        int count;
        int rows[part_dimension];
    };

    struct animation
    {
        timer::time_point started_at;
        unsigned long duration;
    };

    enum class simulation_phase
    {
        dropping,
        clearing_complete_lines,
        gameover_fill,
        gameover_clear,
        gameover_summary,
    };

    struct game_state
    {
        simulation_phase phase;
        moving_piece current;
        tetromino next;
        std::vector<int> piece_bag;
        piece_grid grid;
        int score;
        int level;
        int lines;
        timer::time_point strafe_at;
        int fall_interval_ms;
        timer::time_point fall_at;
        drop_state drop;
        bool rotation_enabled;
        line_completion completion;
    };

    enum class movement
    {
        left,
        right,
        down,
    };

    std::random_device generator;
    game_state state;
    ui::game_layout view;
    animation active_animation;
    SDL_FColor complete_line_color;

    const int piece_count = 7;
    const tetromino piece_templates[piece_count] =
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

    const rotation_offset rotation_offsets[piece_count][rotation_count] =
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
        if (state.piece_bag.empty())
        {
            for (int i = 0; i < piece_count; ++i)
            {
                state.piece_bag.push_back(i);
            }

            std::shuffle(state.piece_bag.begin(), state.piece_bag.end(), generator);
        }

        int piece = state.piece_bag.back();
        state.piece_bag.pop_back();

        return piece;
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

    void spawn_piece()
    {
        copy(state.next, state.current.piece);
        state.current.row = state.grid.height - (state.current.piece.height + 1);
        int center = state.grid.width / 2;
        int offset = (state.current.piece.width / 2) + (state.current.piece.width % 2);
        state.current.column = center - offset;
        state.current.rotation = 0;
        copy(piece_templates[random_piece()], state.next);

        if (collides())
        {
            state.phase = simulation_phase::gameover_fill;
            active_animation =
            {
                .started_at = timer::now(),
                .duration = config.gameplay.field_fill_animation_duration_ms,
            };

            return;
        }

        int fall_level = std::clamp(state.level, 0, static_cast<int>(config.gameplay.fall_intervals.size()));
        const double base_interval = 59.73;
        state.strafe_at = timer::now();
        state.fall_interval_ms = std::round((config.gameplay.fall_intervals[fall_level] / base_interval) * 1000);
        state.fall_at = timer::now() + state.fall_interval_ms;
        state.rotation_enabled = true;
    }

    bool move(movement direction)
    {
        int original_row = state.current.row;
        int original_column = state.current.column;
        
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
            state.drop.rows += state.drop.dropping;
        }

        return state.current.row != original_row
            || state.current.column != original_column;
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

    int fall_interval_ms()
    {
        return state.drop.dropping
            ? std::round((1.0 / config.gameplay.soft_drop_cells) * state.fall_interval_ms)
            : state.fall_interval_ms;
    }

    bool complete_line(int row)
    {
        const int max_bound = state.grid.width - 1;
        int column = 1;

        while (column < max_bound && state.grid.parts[row][column] != 0)
        {
            ++column;
        }

        return column == max_bound;
    }

    void detect_complete_lines()
    {
        int count = 0;

        for (int row = 1; row < state.grid.height; ++row)
        {
            if (complete_line(row))
            {
                state.completion.rows[count++] = row;
            }
        }

        state.completion.count = count;
    }

    void clear_complete_lines()
    {
        int row = 1;

        while (row < state.grid.height && !complete_line(row))
        {
            ++row;
        }

        int complete_count = 0;

        while (row < state.grid.height)
        {
            if (complete_line(row))
            {
                ++complete_count;
            }
            else
            {
                for (int column = 1; column < state.grid.width - 1; ++column)
                {
                    state.grid.parts[row - complete_count][column] = state.grid.parts[row][column];
                }
            }

            ++row;
        }

        for (row = state.grid.height - complete_count; row < state.grid.height; ++row)
        {
            for (int column = 1; column < state.grid.width - 1; ++column)
            {
                state.grid.parts[row][column] = 0;
            }
        }
    }

    void end_commit()
    {
        int line_count = state.completion.count;

        if (line_count > 0)
        {
            state.score += line_count * (state.level + 1) * config.gameplay.line_scores[line_count - 1];
        }

        state.lines += line_count;
        state.level = state.lines / config.gameplay.level_line_requirement;

        state.completion =
        {
            .count = 0,
            .rows = {0, 0, 0, 0},
        };

        state.drop =
        {
            .enabled = !state.drop.dropping,
            .dropping = false,
            .rows = 0,
        };

        clear_complete_lines();
        spawn_piece();
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

        detect_complete_lines();
        state.score += state.drop.rows;

        if (state.completion.count > 0)
        {
            state.phase = simulation_phase::clearing_complete_lines;
            active_animation =
            {
                .started_at = timer::now(),
                .duration = config.gameplay.line_animation_duration_ms,
            };
        }
        else
        {
            end_commit();
        }
    }

    void handle_input()
    {
        if (state.phase == simulation_phase::gameover_summary && input::active("ok"))
        {
            navigation::reload(navigation::scene::game);
            return;
        }

        if (input::finished("drop"))
        {
            state.drop =
            {
                .enabled = true,
                .dropping = false,
                .rows = 0
            };
        }

        if (state.phase != simulation_phase::dropping)
        {
            return;
        }

        if (input::active("strafe_left") && timer::now() >= state.strafe_at)
        {
            move(movement::left);
            state.strafe_at = timer::now() + config.gameplay.strafe_interval;
        }

        if (input::active("strafe_right") && timer::now() >= state.strafe_at)
        {
            move(movement::right);
            state.strafe_at = timer::now() + config.gameplay.strafe_interval;
        }

        if (input::active("drop"))
        {
            if (state.drop.enabled && !state.drop.dropping)
            {
                state.drop.dropping = true;
                state.fall_at = timer::now();
            }
        }

        if (input::active("rotate_left") && state.rotation_enabled)
        {
            rotate_left();

            if (collides())
            {
                rotate_right();
            }
            else
            {
                state.rotation_enabled = false;
            }
        }

        if (input::active("rotate_right") && state.rotation_enabled)
        {
            rotate_right();

            if (collides())
            {
                rotate_left();
            }
            else
            {
                state.rotation_enabled = false;
            }
        }

        if (input::finished("rotate_left") || input::finished("rotate_right"))
        {
            state.rotation_enabled = true;
        }
    }

    void init_state()
    {
        state = game_state
        {
            .phase = simulation_phase::dropping,
            .grid =
            {
                .width = 12,
                .height = 19,
            },
            .score = 0,
            .level = 0,
            .lines = 0,
            .drop =
            {
                .enabled = true,
                .dropping = false,
                .rows = 0,
            },
            .completion =
            {
                .count = 0,
                .rows = {0, 0, 0, 0}
            },
        };

        state.piece_bag.reserve(piece_count);
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
    }

    void init_view()
    {
        view = ui::load_view<ui::game_layout>("game.json");
        view.piece.texture = asset::load_texture(config.piece_types_texture_path);
        complete_line_color = view.grid.complete_line_color;
    }

    void update_state()
    {
        if (state.phase == simulation_phase::dropping && timer::now() >= state.fall_at)
        {
            if (move(movement::down))
            {
                state.fall_at = timer::now() + fall_interval_ms();
            }
            else
            {
                commit();
            }
        }
    }

    void update_view()
    {
        view.current.position =
        {
            .x = view.grid.position.x + (state.current.column * view.piece.width),
            .y = view.grid.position.y - (state.current.row * view.piece.width),
        };

        view.score_value.text = std::to_string(state.score);
        view.level_value.text = std::to_string(state.level);
        view.lines_value.text = std::to_string(state.lines);
        view.current.visible = state.phase < simulation_phase::gameover_fill;
        view.next.visible = state.phase < simulation_phase::gameover_fill;
        view.game_over.visible = state.phase >= simulation_phase::gameover_clear;
        view.top_score.visible = state.phase >= simulation_phase::gameover_clear;
    }

    void animate_complete_lines()
    {
        float ratio = (timer::now() - active_animation.started_at) / static_cast<float>(active_animation.duration);
        complete_line_color.a = std::abs(std::sin(ratio * 4.0f * std::numbers::pi));

        if (ratio >= 0.75f)
        {
            complete_line_color = view.grid.background.color;
        }

        if (ratio >= 1.0)
        {
            state.phase = simulation_phase::dropping;
            end_commit();
            complete_line_color = view.grid.complete_line_color;
        }
    }

    void animate_gameover_fill()
    {
        timer::time_point elapsed = timer::now() - active_animation.started_at;

        if (elapsed <= active_animation.duration / 2)
        {
            float ratio = elapsed / (0.5F * active_animation.duration);
            int field_size = std::round((state.grid.height - 1) * (state.grid.width  - 2) * ratio);
            int i = 0;

            for (int row = 1; i < field_size && row < state.grid.height; ++row)
            {
                for (int column = 1; i < field_size && column < state.grid.width - 1; ++column, ++i)
                {
                    const char filler = 9;
                    state.grid.parts[row][column] = filler;
                }
            }
        }
        else if (elapsed >= active_animation.duration)
        {
            state.phase = simulation_phase::gameover_clear;
            active_animation =
            {
                .started_at = timer::now(),
                .duration = config.gameplay.field_clear_animation_duration_ms,
            };

            score_table scores = scores::load();
            scores::insert(scores, state.score);
            scores::save(scores);

            for (size_t i = 0; i < scores.size(); ++i)
            {
                ui::label l = view.scores.item_template;
                l.text = std::to_string(scores[i]);
                l.position.y += i * view.scores.spacing;
                view.scores.items.push_back(l);
            }
        }
    }

    void animate_gameover_clear()
    {
        float ratio = (timer::now() - active_animation.started_at) / static_cast<float>(active_animation.duration);
        int field_size = std::round((state.grid.height - 1) * (state.grid.width  - 2) * ratio);
        int i = 0;

        for (int row = 1; i < field_size && row < state.grid.height; ++row)
        {
            for (int column = 1; i < field_size && column < state.grid.width - 1; ++column, ++i)
            {
                const char cleaner = 0;
                state.grid.parts[row][column] = cleaner;
            }
        }

        if (ratio >= 1)
        {
            state.phase = simulation_phase::gameover_summary;
        }
    }

    void animate()
    {
        switch (state.phase)
        {
            case simulation_phase::clearing_complete_lines:
                animate_complete_lines();
                break;
            case simulation_phase::gameover_fill:
                animate_gameover_fill();
                break;
            case simulation_phase::gameover_clear:
                animate_gameover_clear();
                break;
            default:
                break;
        }
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
        render::draw_quad(view.grid.background.area, view.grid.background.color);
        render::draw(view.game_over);
        render::draw(view.top_score);

        for (size_t i = 0; i < view.scores.items.size(); ++i)
        {
            render::draw(view.scores.items[i]);
        }

        render::draw(
            render::piece_grid
            {
                .visible = view.current.visible,
                .width = part_dimension,
                .height = part_dimension,
                .position = view.current.position,
                .piece = view.piece,
                .parts = state.current.piece.parts[0],
            });

        render::draw(
            render::piece_grid
            {
                .visible = view.next.visible,
                .width = part_dimension,
                .height = part_dimension,
                .position = view.next.position,
                .piece = view.piece,
                .parts = state.next.parts[0],
            });

        render::draw(
            render::piece_grid
            {
                .visible = view.grid.visible,
                .width = state.grid.width,
                .height = state.grid.height,
                .position = view.grid.position,
                .piece = view.piece,
                .parts = state.grid.parts[0],
            });

        for (int i = 0; i < state.completion.count; ++i)
        {
            SDL_FRect area
            {
                .x = view.grid.position.x + view.piece.width,
                .y = view.grid.position.y - (state.completion.rows[i] * view.piece.width),
                .w = (state.grid.width - 2) * view.piece.width,
                .h = -view.piece.width,
            };

            render::draw_quad(area, complete_line_color);
        }

        render::end_frame();
    }
}

namespace game
{
    void init()
    {
        init_state();
        init_view();
        spawn_piece();
    }

    void run()
    {
        handle_input();
        update_state();
        animate();
        update_view();
        render_view();
    }

    void shutdown()
    {
        asset::unload_texture(view.piece.texture);
        delete[] state.grid.parts[0];
        delete[] state.grid.parts;
    }
}

