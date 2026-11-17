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
        std::string gamepad;
    };

    struct settings_state
    {
        bool vsync;
        std::unordered_map<std::string, key_binding> controls;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(key_binding, keyboard, gamepad)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(settings_state, vsync, controls)
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
    settings.vsync = stored.vsync;

    for (const auto &[action, binding] : stored.controls)
    {
        settings.controls[action] = key_binding
        {
            .keyboard = SDL_GetKeyFromName(binding.keyboard.c_str()),
            .gamepad = SDL_GetGamepadButtonFromString(binding.gamepad.c_str()),
        };
    }
}

