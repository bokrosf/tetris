#include <algorithm>
#include <exception>
#include <format>
#include <vector>
#include <SDL3/SDL.h>
#include <event.h>

namespace
{
    std::vector<SDL_Event> events;
}

namespace event
{
    void init()
    {
        if (!SDL_InitSubSystem(SDL_INIT_EVENTS))
        {
            throw std::runtime_error(std::format("Events Subsystem init failed: {}", SDL_GetError()));
        }
    }

    void shutdown()
    {
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }

    void read_all()
    {
        events.clear();
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            events.push_back(event);
        }
    }

    bool occured(SDL_EventType type)
    {
        auto predicate = [type](const SDL_Event &e) { return e.type == type; };

        return std::any_of(events.begin(), events.end(), predicate);
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
}
