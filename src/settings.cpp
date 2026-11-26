#include <format>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <configuration.h>
#include <serialization.h>
#include <settings.h>
#include <render.h>
#include <ui.h>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(crt_filter_settings, enabled, spacing, color)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(graphics_settings, vsync, crt_filter)

namespace storage
{
    struct key_binding
    {
        std::string keyboard;
        std::string gamepad;
    };

    struct settings_state
    {
        graphics_settings graphics;
        std::unordered_map<std::string, key_binding> controls;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(key_binding, keyboard, gamepad)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(settings_state, graphics, controls)
}

namespace
{
    ui::settings_layout view;
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
    settings.graphics = stored.graphics;

    for (const auto &[action, binding] : stored.controls)
    {
        settings.controls[action] = key_binding
        {
            .keyboard = SDL_GetKeyFromName(binding.keyboard.c_str()),
            .gamepad = SDL_GetGamepadButtonFromString(binding.gamepad.c_str()),
        };
    }
}

void init_settings()
{
    load_settings(config.settings_path);
    view = ui::load_view<ui::settings_layout>(config.views_path / "settings_view.json");
}

void run_settings()
{
    render::begin_frame();
    render::draw(view.title);
    render::end_frame();
}
