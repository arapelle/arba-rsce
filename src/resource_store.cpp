#include <rsce/resource_store.hpp>

namespace rsce
{
std::size_t resource_store_base::filesystem_path_hash::operator()(const std::filesystem::path &arg) const noexcept
{
    using path_string_view = std::basic_string_view<std::filesystem::path::value_type>;
    return std::hash<path_string_view>{}(path_string_view(arg.native()));
}
}
