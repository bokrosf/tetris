#ifndef TETRIS_TIMER_H
#define TETRIS_TIMER_H

namespace timer
{
    using time_point = unsigned long;

    void init();
    void shutdown();
    time_point now();
}

#endif

