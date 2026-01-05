#include <exception>
#include <format>
#include <SDL3/SDL.h>
#include <asset.h>
#include <render.h>
#include <settings.h>

namespace
{
    struct display_mode
    {
        int width;
        int height;
        float height_scale;
    };

    bool initialized = false;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    display_mode display;

    struct piece_grid
    {
        game::dimension width;
        game::dimension height;
        const char *parts;
    };

    float scaled(float value)
    {
        return display.height_scale * value;
    }

    piece_grid to_drawable(const game::tetromino &piece)
    {
        return piece_grid
        {
            .width = game::part_dimension,
            .height = game::part_dimension,
            .parts = piece.parts[0]
        };
    }

    piece_grid to_drawable(const game::piece_grid &grid)
    {
        return piece_grid
        {
            .width = grid.width,
            .height = grid.height,
            .parts = grid.parts[0]
        };
    }

    void draw(const ui::label &label)
    {
        ui::font &font = asset::font(label.font);
        const float symbol_height = scaled(16.0F * label.font_size);
        const float symbol_width = symbol_height * font.width;

        SDL_Texture *texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET,
            symbol_height,
            symbol_height);

        SDL_FRect render_area
        {
            .x = scaled(label.position.x),
            .y = scaled(label.position.y),
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

    void draw(const piece_grid &grid, const SDL_FPoint &position)
    {
        const float width = scaled(30.0);
        const float separator = scaled(5.0);
        SDL_Color original_color;
        SDL_GetRenderDrawColor(renderer, &original_color.r, &original_color.g, &original_color.b, &original_color.a);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

        SDL_FRect area
        {
            .x = scaled(position.x),
            .y = scaled(position.y),
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

            area.x = scaled(position.x);
            area.y -= area.h + separator;
        }

        SDL_SetRenderDrawColor(renderer, original_color.r, original_color.g, original_color.b, original_color.a);
    }
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

        const float base_height = 1080.0F;
        int display_count;
        SDL_DisplayID *display_ids = SDL_GetDisplays(&display_count);
        const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(display_ids[0]);
        display = display_mode
        {
            .width = mode->w,
            .height = mode->h,
            .height_scale = mode->h / base_height
        };

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

    void draw_frame(const game::game_state &state, const ui::game_layout &view)
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);
        draw(view.score);
        draw(to_drawable(state.grid), view.grid);
        draw(to_drawable(state.current), view.current);
        draw(to_drawable(state.next), view.next);
        SDL_RenderPresent(renderer);
    }
}

