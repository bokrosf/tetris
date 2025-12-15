#include <exception>
#include <format>
#include <SDL3/SDL.h>
#include <render.h>
#include <settings.h>

namespace
{
    bool initialized = false;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
}

namespace render
{
    void init()
    {
        if (initialized)
        {
            return;
        }

        if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
        {
            throw std::runtime_error(std::format("Video Subsystem init failed: {}", SDL_GetError()));
        }

        SDL_WindowFlags flags = SDL_WINDOW_FULLSCREEN;

        if (!SDL_CreateWindowAndRenderer(nullptr, 0, 0, flags, &window, &renderer))
        {
            throw std::runtime_error(std::format("Render init failed: {}", SDL_GetError()));
        }

        SDL_SetRenderVSync(renderer, settings.vsync_enabled);
        initialized = true;
    }

    void shutdown()
    {
        if (!initialized)
        {
            return;
        }

        SDL_DestroyWindow(window);
        window = nullptr;

        SDL_DestroyRenderer(renderer);
        renderer = nullptr;

        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        initialized = false;
    }

    void draw_frame()
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
}
