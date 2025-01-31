#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <format>
#include <arba/rsce/basic_resource_manager.hpp>

class text_mngr
{
public:
    std::string contents;

    inline bool operator<=>(const text_mngr&) const = default;

    bool load_from_file(const std::filesystem::path& fpath, rsce::basic_resource_manager&)
    {
        std::ifstream stream(fpath);
        stream.seekg(0, std::ios::end);
        contents.reserve(stream.tellg());
        stream.seekg(0, std::ios::beg);
        contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        return (contents[0] >= 'a' && contents[0] <= 'z')
               || (contents[0] >= 'A' && contents[0] <= 'Z');
    }
};

template<> struct std::hash<text_mngr>
{
    std::size_t operator()(text_mngr const& value) const noexcept { return std::hash<std::string>{}(value.contents); }
};
