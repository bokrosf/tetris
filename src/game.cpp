#include <asset.h>
#include <configuration.h>
#include <game.h>
#include <render.h>
#include <ui.h>

namespace
{
    struct game_state
    {
    };

    game_state state;
    ui::game_layout view;
}

namespace game
{
    void init()
    {
        asset::id_type font = asset::load_font(config.font_path);

        state = game_state();
        view = ui::game_layout
        {
            .score = ui::label
            {
                .text = "Tetris",
                .font = font,
                .font_size = 1,
                .color = SDL_FColor{.r = 0.0, .g = 0.0, .b = 1.0, .a = 1.0},
                .position = SDL_Point{.x = 0, .y = 0}
            }
        };
    }

    void run()
    {
        // TODO: Update game state.
        // TODO: Update view state.
        // TODO: Draw game layout.
        render::draw_frame();
    }
}