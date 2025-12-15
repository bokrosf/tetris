#include <exception>
#include <filesystem>
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

void load_configuration(const std::string &path)
{
    using json = nlohmann::ordered_json;

    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error(std::format("File not found: {}", path));
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

        if (c.settings_path.empty())
        {
            errors.emplace_back("settings_path missing.");
        }

        if (!errors.empty())
        {
            std::ostringstream message;

            for (auto &e : errors)
            {
                message << e << std::endl;
            }

            throw std::runtime_error(std::move(message.str()));
        }
    }
}
