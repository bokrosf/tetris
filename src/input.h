#ifndef TETRIS_INPUT_H
#define TETRIS_INPUT_H

#include <string>

namespace input
{
    bool active(const std::string &action);
    bool finished(const std::string &action);
}

#endif
