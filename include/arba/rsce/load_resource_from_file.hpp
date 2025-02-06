#pragma once

#include "load_resource_from_binary_stream.hpp"
#include "load_resource_from_text_stream.hpp"

#include <filesystem>
#include <fstream>
#include <memory>

inline namespace arba
{
namespace rsce
{

// load_resource_from_file(path);

template <class resource_type>
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path) = delete;

template <class resource_type>
    requires requires(resource_type& value, const std::filesystem::path& fpath) {
        { value.load_from_file(fpath) } -> std::convertible_to<bool>;
    } && (!requires(std::istream& stream) {
                 { load_resource_from_binary_stream<resource_type>(stream) };
             }) && (!requires(std::istream& stream) {
                 { load_resource_from_text_stream<resource_type>(stream) };
             })
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path)
{
    if (std::shared_ptr rsc_sptr = std::make_shared<resource_type>(); rsc_sptr->load_from_file(path)) [[likely]]
        return rsc_sptr;
    return std::shared_ptr<resource_type>();
}

template <class resource_type>
    requires requires(resource_type& value, const std::filesystem::path& fpath) {
        { value.load_from_file(fpath) } -> std::same_as<void>;
    } && (!requires(std::istream& stream) {
                 { load_resource_from_binary_stream<resource_type>(stream) };
             }) && (!requires(std::istream& stream) {
                 { load_resource_from_text_stream<resource_type>(stream) };
             })
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path)
{
    std::shared_ptr rsc_sptr = std::make_shared<resource_type>();
    rsc_sptr->load_from_file(path);
    return rsc_sptr;
}

template <class resource_type>
    requires requires(std::istream& stream) {
        { load_resource_from_binary_stream<resource_type>(stream) } -> std::same_as<std::shared_ptr<resource_type>>;
    }
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ifstream::binary);
    stream.exceptions(std::ifstream::failbit);
    return load_resource_from_binary_stream<resource_type>(stream);
}

template <class resource_type>
    requires requires(std::istream& stream) {
        { load_resource_from_text_stream<resource_type>(stream) } -> std::same_as<std::shared_ptr<resource_type>>;
    }
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path)
{
    std::ifstream stream(path);
    stream.exceptions(std::ifstream::failbit);
    return load_resource_from_text_stream<resource_type>(stream);
}

// load_resource_from_file(path, resource_manager);

template <class resource_type, class resource_manager_type>
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path,
                                                       resource_manager_type& rsc_manager) = delete;

template <class resource_type, class resource_manager_type>
    requires requires(resource_type& value, const std::filesystem::path& fpath, resource_manager_type& rsc_manager) {
        { value.load_from_file(fpath, rsc_manager) } -> std::convertible_to<bool>;
    } && (!requires(std::istream& stream, resource_manager_type& rsc_manager) {
                 { load_resource_from_binary_stream<resource_type>(stream, rsc_manager) };
             }) && (!requires(std::istream& stream, resource_manager_type& rsc_manager) {
                 { load_resource_from_text_stream<resource_type>(stream, rsc_manager) };
             })
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path,
                                                       resource_manager_type& rsc_manager)
{
    if (std::shared_ptr rsc_sptr = std::make_shared<resource_type>(); rsc_sptr->load_from_file(path, rsc_manager))
        [[likely]]
        return rsc_sptr;
    return std::shared_ptr<resource_type>();
}

template <class resource_type, class resource_manager_type>
    requires requires(resource_type& value, const std::filesystem::path& fpath, resource_manager_type& rsc_manager) {
        { value.load_from_file(fpath, rsc_manager) } -> std::same_as<void>;
    } && (!requires(std::istream& stream, resource_manager_type& rsc_manager) {
                 { load_resource_from_binary_stream<resource_type>(stream, rsc_manager) };
             }) && (!requires(std::istream& stream, resource_manager_type& rsc_manager) {
                 { load_resource_from_text_stream<resource_type>(stream, rsc_manager) };
             })
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path,
                                                       resource_manager_type& rsc_manager)
{
    std::shared_ptr rsc_sptr = std::make_shared<resource_type>();
    rsc_sptr->load_from_file(path, rsc_manager);
    return rsc_sptr;
}

template <class resource_type, class resource_manager_type>
    requires requires(std::istream& stream, resource_manager_type& rsc_manager) {
        {
            load_resource_from_binary_stream<resource_type>(stream, rsc_manager)
        } -> std::same_as<std::shared_ptr<resource_type>>;
    }
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path,
                                                       resource_manager_type& rsc_manager)
{
    std::ifstream stream(path, std::ifstream::binary);
    stream.exceptions(std::ifstream::failbit);
    return load_resource_from_binary_stream<resource_type>(stream, rsc_manager);
}

template <class resource_type, class resource_manager_type>
    requires requires(std::istream& stream, resource_manager_type& rsc_manager) {
        {
            load_resource_from_text_stream<resource_type>(stream, rsc_manager)
        } -> std::same_as<std::shared_ptr<resource_type>>;
    }
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path,
                                                       resource_manager_type& rsc_manager)
{
    std::ifstream stream(path);
    stream.exceptions(std::ifstream::failbit);
    return load_resource_from_text_stream<resource_type>(stream, rsc_manager);
}

namespace concepts
{
template <class resource_type>
concept loadable_resource = requires(resource_type& value, const std::filesystem::path& fpath) {
    { load_resource_from_file<resource_type>(fpath) } -> std::convertible_to<std::shared_ptr<resource_type>>;
};

template <class resource_type, class resource_manager_type>
concept loadable_with_manager_resource =
    requires(resource_type& value, const std::filesystem::path& fpath, resource_manager_type& rsc_manager) {
        {
            load_resource_from_file<resource_type>(fpath, rsc_manager)
        } -> std::convertible_to<std::shared_ptr<resource_type>>;
    };
} // namespace concepts

namespace traits
{
template <class resource_type, class resource_manager_type = void>
inline constexpr bool is_loadable_resource_v = false;

template <class resource_type, class resource_manager_type>
    requires(!std::is_same_v<resource_manager_type, void>)
                && concepts::loadable_with_manager_resource<resource_type, resource_manager_type>
inline constexpr bool is_loadable_resource_v<resource_type, resource_manager_type> = true;

template <class resource_type>
    requires concepts::loadable_resource<resource_type>
inline constexpr bool is_loadable_resource_v<resource_type, void> = true;
} // namespace traits

} // namespace rsce
} // namespace arba
