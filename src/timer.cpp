#include <format>
#include <stdexcept>
#include <SDL3/SDL.h>
#include <timer.h>

namespace timer
{
    void init()
    {
        if (!SDL_Init(SDL_INIT_EVENTS))
        {
            throw std::runtime_error(std::format("Timer Subsysytem init failed: {}", SDL_GetError()));
        }
    }

    void shutdown()
    {
        SDL_QuitSubSystem(SDL_INIT_EVENTS);
    }

    time_point now()
    {
        return SDL_GetTicks();
    }
}

