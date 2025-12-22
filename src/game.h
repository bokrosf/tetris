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

    struct piece_grid
    {
        dimension width;
        dimension height;
        char **parts;
    };

    struct game_state
    {
        tetromino current;
        tetromino next;
        piece_grid grid;
    };

    void init();
    void run();
    void shutdown();
}

#endif
