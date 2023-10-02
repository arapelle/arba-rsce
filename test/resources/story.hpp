#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <format>
#include <arba/rsce/basic_resource_manager.hpp>

class story
{
public:
    std::string contents;

    inline bool operator<=>(const story&) const = default;

    void load_from_file(const std::filesystem::path& fpath)
    {
        std::ifstream stream(fpath);
        stream.seekg(0, std::ios::end);
        contents.reserve(stream.tellg());
        stream.seekg(0, std::ios::beg);
        contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        if (!(contents[0] >= 'a' && contents[0] <= 'z'
              || contents[0] >= 'A' && contents[0] <= 'Z'))
        {
            std::string err_msg = std::format("Problem to load \"{}\" as an input stream.", fpath.generic_string());
            throw std::runtime_error(err_msg);
        }
    }
};

template<> struct std::hash<story>
{
    std::size_t operator()(text const& value) const noexcept { return std::hash<std::string>{}(value.contents); }
};
