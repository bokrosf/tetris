#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <font.h>

font load_font(const std::string &path)
{
    using json = nlohmann::ordered_json;

    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error(std::format("Font not found: {}", path));
    }

    std::ifstream input(path);
    json json_data = json::parse(input);
    
    return json_data.get<font>();
}
