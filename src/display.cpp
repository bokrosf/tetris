#include <stdexcept>
#include <SDL3/SDL.h>
#include <display.h>

namespace
{
    struct display_mode
    {
        int width;
        int height;
        float height_scale;
    };

    const float base_height = 1080.0F;
    const float base_font_height = 16.0F;

}

namespace display
{
    display_mode mode;

    void init()
    {
        int display_count;
        SDL_DisplayID *display_ids = SDL_GetDisplays(&display_count);

        if (display_count < 1)
        {
            throw std::runtime_error("Not found any display. At least one display required.");
        }

        const SDL_DisplayMode *current = SDL_GetCurrentDisplayMode(display_ids[0]);
        SDL_free(display_ids);

        mode = display_mode
        {
            .width = current->w,
            .height = current->h,
            .height_scale = current->h / base_height
        };
    }

    float scaled(float value)
    {
        return mode.height_scale * value;
    }

    float scaled_font_height(float height)
    {
        return scaled(base_font_height * height);
    }
}

