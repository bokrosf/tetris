#include <exception>
#include <format>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <settings.h>

namespace storage
{
    struct key_binding
    {
        std::string keyboard;
        std::string controller;
    };

    struct settings_state
    {
        bool vsync_enabled;
        std::unordered_map<std::string, key_binding> controls;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(key_binding, keyboard, controller)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(settings_state, vsync_enabled, controls)
}

settings_state settings;

void load_settings(const std::filesystem::path &path)
{
    using json = nlohmann::ordered_json;

    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error(std::format("Settings file not found: {}", path.c_str()));
    }

    std::ifstream input(path);
    json json_data = json::parse(input);
    storage::settings_state stored = json_data.get<storage::settings_state>();
    settings.vsync_enabled = stored.vsync_enabled;

    for (const auto &[action, binding] : stored.controls)
    {
        settings.controls[action] = key_binding
        {
            .keyboard = SDL_GetKeyFromName(binding.keyboard.c_str()),
            .controller = SDL_GetGamepadButtonFromString(binding.controller.c_str()),
        };
    }
}

