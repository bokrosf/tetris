#include <algorithm>
#include <cmath>
#include <exception>
#include <format>
#include <SDL3/SDL.h>
#include <asset.h>
#include <display.h>
#include <render.h>
#include <settings.h>

namespace
{
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
}

namespace render
{
    void init()
    {
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
    }

    void shutdown()
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;

        SDL_DestroyWindow(window);
        window = nullptr;

        SDL_QuitSubSystem(SDL_INIT_VIDEO);
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
        const float width = grid.arguments.width;
        SDL_Texture &texture = asset::texture(grid.arguments.texture);
        SDL_SetTextureScaleMode(&texture, SDL_SCALEMODE_NEAREST);

        for (game::dimension row = 0; row < grid.height; ++row)
        {
            for (game::dimension column = 0; column < grid.width; ++column)
            {
                if (char piece_type = grid.parts[(row * grid.width) + column])
                {
                    SDL_FRect texture_area
                    {
                        .x = static_cast<float>(texture.h) * (piece_type - 1),
                        .y = 0,
                        .w = static_cast<float>(texture.h),
                        .h = static_cast<float>(texture.h),
                    };

                    SDL_FRect part_area
                    {
                        .x = grid.position.x + (column * width),
                        .y = grid.position.y - (row * width),
                        .w = width,
                        .h = -width,
                    };

                    SDL_RenderTexture(renderer, &texture, &texture_area, &part_area);
                }
            }
        }
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

    void draw_texture(asset::id_type id, const SDL_FRect &area)
    {
        SDL_Texture &texture = asset::texture(id);
        SDL_SetTextureScaleMode(&texture, SDL_SCALEMODE_NEAREST);
        float scale = std::round(std::min(area.w / texture.w, area.h / texture.h));
        SDL_RenderTextureTiled(renderer, &texture, nullptr, scale, &area);
    }

    SDL_Texture *load_texture(SDL_Surface *surface)
    {
        return SDL_CreateTextureFromSurface(renderer, surface);
    }
}

