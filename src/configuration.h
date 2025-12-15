#ifndef TETRIS_CONFIGURATION_H
#define TETRIS_CONFIGURATION_H

#include <string>
#include <nlohmann/json.hpp>
#include <SDL3/SDL.h>

struct configuration
{
    std::string font_path;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configuration, font_path)

#endif
