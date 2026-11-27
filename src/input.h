#ifndef TETRIS_INPUT_H
#define TETRIS_INPUT_H

#include <string>

namespace input
{
    void init();
    void react_to_events();
    bool active(const std::string &action);
    bool finished(const std::string &action);
    void suspend_tracking();
    void continue_tracking();
    void finish_all_actions();
}

#endif
