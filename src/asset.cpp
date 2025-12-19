#include <nlohmann/json.hpp>
#include <asset.h>

namespace asset
{
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(storage::font_vertex, x, y)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(storage::symbol, code, vertices)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(storage::font, width, symbols)
}
