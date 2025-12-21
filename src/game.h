#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

namespace game
{
    using dimension = unsigned int;
    const dimension part_dimension = 4;

    struct tetromino
    {
        dimension width;
        dimension height;
        char parts[part_dimension][part_dimension];
    };

    struct game_state
    {
        tetromino current;
        tetromino next;
    };

    void init();
    void run();
}

#endif
