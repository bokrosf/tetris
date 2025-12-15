#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>
#include <settings.h>
#include <vector>

namespace
{
    void validate(const settings_state &s);
}

settings_state settings;

void load_settings(const std::string &path)
{
    using json = nlohmann::ordered_json;

    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error(std::format("Settings file not found: {}", path));
    }

    std::ifstream input(path);
    json json_data = json::parse(input);
    settings_state settings_data = json_data.get<settings_state>();
    validate(settings_data);

    settings = std::move(settings_data);
}

namespace
{
    void validate(const settings_state &s)
    {
        std::vector<std::string> errors;

        // Implement value checks.

        if (!errors.empty())
        {
            std::ostringstream message("Invalid settings. ");

            for (const auto &e : errors)
            {
                message << e << std::endl;
            }

            throw std::runtime_error(std::move(message.str()));
        }
    }
}
