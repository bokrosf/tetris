#ifndef TETRIS_CONFIGURATION_H
#define TETRIS_CONFIGURATION_H

#include <string>
#include <nlohmann/json.hpp>

struct configuration
{
    std::string font_path;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(configuration, font_path)

extern configuration config;

void load_configuration(const std::string &path);

#endif
