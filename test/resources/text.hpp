#pragma once

#include <arba/rsce/basic_resource_manager.hpp>

#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>

class text
{
public:
    std::string contents;

    inline bool operator<=>(const text&) const = default;

    bool load_from_file(const std::filesystem::path& fpath)
    {
        std::ifstream stream(fpath);
        stream.seekg(0, std::ios::end);
        contents.reserve(stream.tellg());
        stream.seekg(0, std::ios::beg);
        contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        return (contents[0] >= 'a' && contents[0] <= 'z') || (contents[0] >= 'A' && contents[0] <= 'Z');
    }
};

template <>
struct std::hash<text>
{
    std::size_t operator()(text const& value) const noexcept { return std::hash<std::string>{}(value.contents); }
};
