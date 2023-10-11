#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <format>

class stream_binary_rsc
{
public:
    std::string contents;

    inline bool operator<=>(const stream_binary_rsc&) const = default;

    bool load_from_binary_stream(std::istream& stream)
    {
        stream.seekg(0, std::ios::end);
        contents.reserve(stream.tellg());
        stream.seekg(0, std::ios::beg);
        contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        return contents[0] >= 'a' && contents[0] <= 'z'
            || contents[0] >= 'A' && contents[0] <= 'Z';
    }

    bool load_from_file(const std::filesystem::path&)
    {
        return false;
    }
};

template<> struct std::hash<stream_binary_rsc>
{
    std::size_t operator()(stream_binary_rsc const& value) const noexcept { return std::hash<std::string>{}(value.contents); }
};

class throw_stream_binary_rsc
{
public:
    std::string contents;

    inline bool operator<=>(const throw_stream_binary_rsc&) const = default;

    void load_from_binary_stream(std::istream& stream)
    {
        stream.seekg(0, std::ios::end);
        contents.reserve(stream.tellg());
        stream.seekg(0, std::ios::beg);
        contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        if (!(contents[0] >= 'a' && contents[0] <= 'z'
              || contents[0] >= 'A' && contents[0] <= 'Z'))
        {
            throw std::runtime_error("Problem to load.");
        }
    }

    void load_from_file(const std::filesystem::path&)
    {
        throw std::exception();
    }
};

template<> struct std::hash<throw_stream_binary_rsc>
{
    std::size_t operator()(throw_stream_binary_rsc const& value) const noexcept { return std::hash<std::string>{}(value.contents); }
};
