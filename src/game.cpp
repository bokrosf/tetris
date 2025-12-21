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

    void copy(tetromino &from, tetromino &to)
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

