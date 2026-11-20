#include <exception>
#include <format>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <configuration.h>

configuration config;

namespace
{
    void validate(const configuration &c);
}

void load_configuration(const std::filesystem::path &path)
{
    using json = nlohmann::ordered_json;

    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error(std::format("Configuration file not found: {}", path.c_str()));
    }

    std::ifstream input(path);
    json config_json = json::parse(input);
    configuration loaded = config_json.get<configuration>();
    validate(loaded);
    config = std::move(loaded);
}

namespace
{
    void validate(const configuration &c)
    {
        std::vector<std::string> errors;

        if (c.font_path.empty())
        {
            errors.emplace_back("font_path missing.");
        }

        if (c.piece_types_texture_path.empty())
        {
            errors.emplace_back("piece_types_texture_path missing.");
        }

        if (c.settings_path.empty())
        {
            errors.emplace_back("settings_path missing.");
        }

        if (c.gameplay.level_line_requirement < 1)
        {
            errors.emplace_back("level_line_requirement must be greater than 0.");
        }

        if (c.gameplay.soft_drop_cells < 1)
        {
            errors.emplace_back("soft_drop_cells must be greater than 0.");
        }

        if (c.ui.design_height < 1)
        {
            errors.emplace_back("design_height must be greater than 0.");
        }

        if (c.ui.font_height < 1)
        {
            errors.emplace_back("font_height must be greater than 0.");
        }

        if (c.ui.aspect_ratio.width < 1)
        {
            errors.emplace_back("aspect_ratio.width must be greater than 0.");
        }

        if (c.ui.aspect_ratio.height < 1)
        {
            errors.emplace_back("aspect_ratio.height must be greater than 0.");
        }

        if (!errors.empty())
        {
            std::ostringstream message;
            message << "Configuration errors: " << std::endl;

            for (auto &e : errors)
            {
                message << "  - " << e << std::endl;
            }

            throw std::runtime_error(std::move(message.str()));
        }
    }
}
