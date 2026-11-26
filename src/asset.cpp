#include <exception>
#include <format>
#include <fstream>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <asset.h>
#include <configuration.h>
#include <font.h>
#include <render.h>

using json = nlohmann::ordered_json;

namespace
{
    using namespace asset;
    const id_type initial_id_value = 0;

    id_type last_id;
    std::unordered_map<id_type, ui::font> fonts;
    std::unordered_map<id_type, SDL_Texture *> textures;
}

namespace asset
{
    void init()
    {
        last_id = initial_id_value;
        load_font(config.font_path);
    }

    id_type load_font(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error(std::format("Font asset not existing: {}", path.c_str()));
        }

        std::ifstream input(path);
        json json_data = json::parse(input);
        ui::storage::font stored = json_data.get<ui::storage::font>();
        ui::font loaded{.width = stored.width};
        loaded.symbols.reserve(stored.symbols.size());

        for (const auto &symbol : stored.symbols)
        {
            loaded.symbols[symbol.code].reserve(stored.symbols.size());

            for (const auto &v : symbol.vertices)
            {
                loaded.symbols[symbol.code].push_back(
                    SDL_Vertex
                    {
                        .position{.x = v.x, .y = v.y},
                    });
            }
        }

        ++last_id;
        fonts[last_id] = std::move(loaded);

        return last_id;
    }

    id_type load_texture(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            throw std::runtime_error(std::format("Texture asset not existing: {}", path.c_str()));
        }

        SDL_Surface *surface = SDL_LoadBMP(path.c_str());

        if (!surface)
        {
            throw std::runtime_error(std::format("Texture asset loading failed. Error: {}, path: {}", SDL_GetError(), path.c_str()));
        }

        SDL_Texture *texture = render::load_texture(surface);

        if (!texture)
        {
            throw std::runtime_error(std::format("Texture asset loading failed. Error: {}, path: {}", SDL_GetError(), path.c_str()));
        }

        SDL_DestroySurface(surface);
        ++last_id;
        textures[last_id] = texture;

        return last_id;
    }

    void unload_texture(id_type id)
    {
        if (textures.contains(id))
        {
            render::unload_texture(textures[id]);
            textures.erase(id);
        }
    }

    void unload_all()
    {
        fonts.clear();

        for (auto [id, t] : textures)
        {
            SDL_DestroyTexture(t);
        }
    }

    ui::font &font(id_type id)
    {
        return fonts.at(id);
    }

    SDL_Texture &texture(id_type id)
    {
        return *textures.at(id);
    }
}
