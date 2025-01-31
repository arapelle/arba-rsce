#pragma once

#include <arba/rsce/basic_resource_manager.hpp>

#include <filesystem>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>

class stream_binary_rsc_mngr
{
public:
    std::string contents;

    inline bool operator<=>(const stream_binary_rsc_mngr&) const = default;

    bool load_from_binary_stream(std::istream& stream, rsce::basic_resource_manager&)
    {
        stream.seekg(0, std::ios::end);
        contents.reserve(stream.tellg());
        stream.seekg(0, std::ios::beg);
        contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        return (contents[0] >= 'a' && contents[0] <= 'z') || (contents[0] >= 'A' && contents[0] <= 'Z');
    }

    bool load_from_file(const std::filesystem::path&, rsce::basic_resource_manager&) { return false; }
};

template <>
struct std::hash<stream_binary_rsc_mngr>
{
    std::size_t operator()(stream_binary_rsc_mngr const& value) const noexcept
    {
        return std::hash<std::string>{}(value.contents);
    }
};

class throw_stream_binary_rsc_mngr
{
public:
    std::string contents;

    inline bool operator<=>(const throw_stream_binary_rsc_mngr&) const = default;

    void load_from_binary_stream(std::istream& stream, rsce::basic_resource_manager&)
    {
        stream.seekg(0, std::ios::end);
        contents.reserve(stream.tellg());
        stream.seekg(0, std::ios::beg);
        contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        if (!((contents[0] >= 'a' && contents[0] <= 'z') || (contents[0] >= 'A' && contents[0] <= 'Z')))
        {
            throw std::runtime_error("Problem to load.");
        }
    }

    void load_from_file(const std::filesystem::path&, rsce::basic_resource_manager&) { throw std::exception(); }
};

template <>
struct std::hash<throw_stream_binary_rsc_mngr>
{
    std::size_t operator()(throw_stream_binary_rsc_mngr const& value) const noexcept
    {
        return std::hash<std::string>{}(value.contents);
    }
};
