#include <algorithm>
#include <exception>
#include <format>
#include <vector>
#include <SDL3/SDL.h>
#include <event.h>

#include <iostream>

namespace
{
    std::vector<SDL_Event> events;
    std::vector<SDL_Gamepad *> gamepads;
}

namespace event
{
    void init()
    {
        if (!SDL_InitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMEPAD))
        {
            throw std::runtime_error(std::format("Events Subsystem init failed: {}", SDL_GetError()));
        }
    }

    void shutdown()
    {
        for (auto gp : gamepads)
        {
            SDL_CloseGamepad(gp);
        }

        SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_GAMEPAD);
    }

    void read_all()
    {
        events.clear();
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            events.push_back(event);

            if (event.type == SDL_EVENT_GAMEPAD_ADDED)
            {
                gamepads.push_back(SDL_OpenGamepad(event.gdevice.which));
            }
        }
    }

    bool occured(SDL_EventType type)
    {
        auto predicate = [type](const SDL_Event &e) { return e.type == type; };

        return std::any_of(events.begin(), events.end(), predicate);
    }

    const std::vector<SDL_Event> &queue()
    {
        return events;
    }

    bool key_down(SDL_Keycode key)
    {
        auto predicate = [key](const SDL_Event &e)
        {
            return e.type == SDL_EVENT_KEY_DOWN && e.key.key == key;
        };

        return std::any_of(events.begin(), events.end(), predicate);
    }

    bool key_up(SDL_Keycode key)
    {
        auto predicate = [key](const SDL_Event &e)
        {
            return e.type == SDL_EVENT_KEY_UP && e.key.key == key;
        };

        return std::any_of(events.begin(), events.end(), predicate);
    }

    bool button_down(SDL_GamepadButton button)
    {
        auto predicate = [button](const SDL_Event &e)
        {
            return e.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN
                && e.gbutton.button == button;
        };

        return std::any_of(events.begin(), events.end(), predicate);
    }

    bool button_up(SDL_GamepadButton button)
    {
        auto predicate = [button](const SDL_Event &e)
        {
            return e.type == SDL_EVENT_GAMEPAD_BUTTON_UP
                && e.gbutton.button == button;
        };

        return std::any_of(events.begin(), events.end(), predicate);
    }
}
