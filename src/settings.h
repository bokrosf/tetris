#ifndef TETRIS_SETTINGS_H
#define TETRIS_SETTINGS_H

#include <string>
#include <nlohmann/json.hpp>

struct settings_state
{
    bool vsync_enabled;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(settings_state, vsync_enabled)

extern settings_state settings;

void load_settings(const std::string &path);

#endif
