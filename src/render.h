#ifndef TETRIS_RENDER_H
#define TETRIS_RENDER_H

#include <game.h>
#include <ui.h>

namespace render
{
    void init();
    void shutdown();
    void draw_frame(const game::game_state &state, const ui::game_layout &view);
}

#endif
