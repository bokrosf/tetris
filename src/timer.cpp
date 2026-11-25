#include <unordered_map>
#include <format>
#include <stdexcept>
#include <SDL3/SDL.h>
#include <timer.h>

namespace
{
    using namespace timer;

    struct context
    {
        int id;
        time_point inactivity;
        time_point suspended_at;
    };

    const int app_context_id = 0;

    int last_id;
    time_point frame_start;
    std::unordered_map<int, context> contexts;
    context *active;
}

namespace timer
{
    void init()
    {
        if (!SDL_Init(SDL_INIT_EVENTS))
        {
            throw std::runtime_error(std::format("Timer Subsysytem init failed: {}", SDL_GetError()));
        }

        last_id = 0;
        frame_start = 0;
        contexts.clear();

        contexts[app_context_id] =
        {
            .id = app_context_id,
            .inactivity = 0,
            .suspended_at = 0,
        };

        active = &contexts[app_context_id];
    }

    void shutdown()
    {
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }

    void advance()
    {
        frame_start = SDL_GetTicks();
    }

    time_point now()
    {
        return frame_start - active->inactivity;
    }

    void switch_context(int id)
    {
        if (id == app_context_id)
        {
            return;
        }

        if (active && active->id == id)
        {
            return;
        }

        if (active)
        {
            active->suspended_at = frame_start;
        }

        if (contexts.contains(id))
        {
            context &c = contexts[id];
            c.inactivity += frame_start - c.suspended_at;
        }
        else
        {
            contexts[id] =
            {
                .id = ++last_id,
                .inactivity = 0,
            };
        }

        active = &contexts[id];
    }

    void remove_context(int id)
    {
        if (id == app_context_id)
        {
            return;
        }

        contexts.erase(id);

        if (active && active->id == id)
        {
            active = &contexts[app_context_id];
        }
    }
}

