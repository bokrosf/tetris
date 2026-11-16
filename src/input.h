#ifndef TETRIS_INPUT_H
#define TETRIS_INPUT_H

#include <string>

namespace input
{
    void track_actions();
    void react_to_events();
    bool active(const std::string &action);
    bool finished(const std::string &action);
}

#endif
