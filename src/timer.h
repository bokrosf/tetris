#ifndef TETRIS_TIMER_H
#define TETRIS_TIMER_H

namespace timer
{
    using time_point = unsigned long;

    void init();
    void shutdown();
    void advance();
    time_point now();
}

#endif

