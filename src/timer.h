#ifndef TETRIS_TIMER_H
#define TETRIS_TIMER_H

namespace timer
{
    using time_point = unsigned long;

    void init();
    void shutdown();
    void advance();
    time_point now();
    void switch_context(int id);
    void remove_context(int id);
}

#endif

