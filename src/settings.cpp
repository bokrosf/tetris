#include <format>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <nlohmann/json.hpp>
#include <configuration.h>
#include <input.h>
#include <navigation.h>
#include <serialization.h>
#include <settings.h>
#include <render.h>
#include <ui.h>

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(crt_filter_settings, enabled, spacing, color)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(graphics_settings, fullscreen, vsync, crt_filter)

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

    ui::checkbox *create_checkbox(bool *tracked)
    {
        ui::checkbox *cb = new ui::checkbox;
        *cb = view.checkbox_template;
        cb->checked = tracked;

        return cb;
    }
}

settings_state settings;

using json = nlohmann::ordered_json;

void load_settings(const std::filesystem::path &path)
{
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

    settings.controls["menu_down"] = {.keyboard = SDLK_DOWN, .gamepad = SDL_GAMEPAD_BUTTON_DPAD_DOWN};
    settings.controls["menu_up"] = {.keyboard = SDLK_UP, .gamepad = SDL_GAMEPAD_BUTTON_DPAD_UP};
    settings.controls["ok"] = {.keyboard = SDLK_RETURN, .gamepad = SDL_GAMEPAD_BUTTON_SOUTH};
    settings.controls["cancel"] = {.keyboard = SDLK_ESCAPE, .gamepad = SDL_GAMEPAD_BUTTON_EAST};
    settings.controls["pause"] = {.keyboard = SDLK_ESCAPE, .gamepad = SDL_GAMEPAD_BUTTON_START};
}

void save_settings()
{
    render::apply_settings();
}

void init_settings()
{
    load_settings(config.settings_path);
    view = ui::load_view<ui::settings_layout>("settings.json");

    view.options.push_back(
        {
            .toggle = create_checkbox(&settings.graphics.fullscreen),
            .binding = nullptr,
        }
    );

    view.options.push_back(
        {
            .toggle = create_checkbox(&settings.graphics.vsync),
            .binding = nullptr,
        }
    );

    view.options.push_back(
        {
            .toggle = create_checkbox(&settings.graphics.crt_filter.enabled),
            .binding = nullptr,
        }
    );

    for (size_t i = 0; i < view.options.size(); ++i)
    {
        view.options[i].toggle->position.y += i * view.spacing;
    }
}

void run_settings()
{
    if (input::active("cancel"))
    {
        navigation::pop();
    }

    render::begin_frame();
    render::draw(view.title);
    render::draw(view.graphics);
    render::draw(view.controls);

    for (const auto &item : view.options)
    {
        if (item.toggle)
        {
            render::draw(*item.toggle);
        }
        else if (item.binding)
        {

        }
    }

    render::end_frame();
}

void shutdown_settings()
{
    for (auto &item : view.options)
    {
        delete item.toggle;
        item.toggle = nullptr;

        delete item.binding;
        item.binding = nullptr;
    }
}
