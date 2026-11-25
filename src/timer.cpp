#include <format>
#include <stdexcept>
#include <SDL3/SDL.h>
#include <timer.h>

namespace
{
    using namespace timer;

    time_point frame_start;
}

namespace timer
{
    void init()
    {
        frame_start = 0;

        if (!SDL_Init(SDL_INIT_EVENTS))
        {
            throw std::runtime_error(std::format("Timer Subsysytem init failed: {}", SDL_GetError()));
        }
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
        return frame_start;
    }
}

