#include <exception>
#include <format>
#include <SDL3/SDL.h>
#include <asset.h>
#include <display.h>
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

    void begin_frame()
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
    }

    void end_frame()
    {
        SDL_RenderPresent(renderer);
    }

    void draw(const piece_grid &grid)
    {
        const float width = display::scaled(grid.arguments.width);
        const float separator = display::scaled(grid.arguments.separator);
        SDL_Color original_color;
        SDL_GetRenderDrawColor(renderer, &original_color.r, &original_color.g, &original_color.b, &original_color.a);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

        SDL_FRect area
        {
            .x = display::scaled(grid.position.x),
            .y = display::scaled(grid.position.y),
            .w = width,
            .h = width,
        };

        for (game::dimension row = 0; row < grid.height; ++row)
        {
            for (game::dimension column = 0; column < grid.width; ++column)
            {
                if (grid.parts[(row * grid.width) + column])
                {
                    SDL_RenderFillRect(renderer, &area);
                }

                area.x += area.w + separator;
            }

            area.x = display::scaled(grid.position.x);
            area.y -= area.h + separator;
        }

        SDL_SetRenderDrawColor(renderer, original_color.r, original_color.g, original_color.b, original_color.a);
    }

    void draw(const ui::label &label)
    {
        ui::font &font = asset::font(label.font);
        const float symbol_height = display::scaled_font_height(label.font_size);
        const float symbol_width = symbol_height * font.width;

        SDL_Texture *texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET,
            symbol_height,
            symbol_height);

        SDL_FRect render_area
        {
            .x = display::scaled(label.position.x),
            .y = display::scaled(label.position.y),
            .w = symbol_width,
            .h = symbol_height
        };

        for (auto symbol : label.text)
        {
            auto &vertices = font.symbols[symbol];

            for (auto &v : vertices)
            {
                v.color = label.color;
            }

            SDL_SetRenderTarget(renderer, texture);
            SDL_RenderClear(renderer);
            SDL_SetRenderScale(renderer, symbol_height, symbol_height);
            SDL_RenderGeometry(renderer, nullptr, vertices.data(), vertices.size(), nullptr, 0);
            SDL_SetRenderTarget(renderer, nullptr);

            float empty_offset = 0.5 * (symbol_height - symbol_width);
            SDL_FRect texture_area{.x = empty_offset, .y = 0, .w = symbol_width, .h = symbol_height};
            SDL_RenderTexture(renderer, texture, &texture_area, &render_area);
            render_area.x += symbol_width;
        }

        SDL_DestroyTexture(texture);
        SDL_SetRenderTarget(renderer, nullptr);
    }
}

