#pragma once

#include <filesystem>
#include <memory>

inline namespace arba
{
namespace rsce
{

template <class resource_type>
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path) = delete;

template <class resource_type>
requires requires(resource_type& value, const std::filesystem::path& fpath)
{
    { value.load_from_file(fpath) } -> std::convertible_to<bool>;
}
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path)
{
    resource_type value;
    if (value.load_from_file(path))
        return std::make_shared<resource_type>(std::move(value));
    return std::shared_ptr<resource_type>();
}

template <class resource_type>
    requires requires(resource_type& value, const std::filesystem::path& fpath)
{
    { value.load_from_file(fpath) } -> std::same_as<void>;
}
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path)
{
    resource_type value;
    value.load_from_file(path);
    return std::make_shared<resource_type>(std::move(value));
}

template <class resource_type, class resource_manager_type>
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path, resource_manager_type& rsc_manager) = delete;

template <class resource_type, class resource_manager_type>
requires requires(resource_type& value, const std::filesystem::path& fpath, resource_manager_type& rsc_manager)
{
    { value.load_from_file(fpath, rsc_manager) } -> std::convertible_to<bool>;
}
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path, resource_manager_type &rsc_manager)
{
    resource_type value;
    if (value.load_from_file(path, rsc_manager))
        return std::make_shared<resource_type>(std::move(value));
    return std::shared_ptr<resource_type>();
}

template <class resource_type, class resource_manager_type>
    requires requires(resource_type& value, const std::filesystem::path& fpath, resource_manager_type& rsc_manager)
{
    { value.load_from_file(fpath, rsc_manager) } -> std::same_as<void>;
}
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path, resource_manager_type &rsc_manager)
{
    resource_type value;
    value.load_from_file(path, rsc_manager);
    return std::make_shared<resource_type>(std::move(value));
}

namespace concepts
{
template <class resource_type>
concept loadable_resource = requires(resource_type& value, const std::filesystem::path& fpath)
{
    { load_resource_from_file<resource_type>(fpath) } -> std::convertible_to<std::shared_ptr<resource_type>>;
};

template <class resource_type, class resource_manager_type>
concept loadable_with_manager_resource = requires(resource_type& value, const std::filesystem::path& fpath,
                                                  resource_manager_type& rsc_manager)
{
    { load_resource_from_file<resource_type>(fpath, rsc_manager) } -> std::convertible_to<std::shared_ptr<resource_type>>;
};
}

namespace traits
{
template <class resource_type, class resource_manager_type = void>
inline constexpr bool is_loadable_resource_v = false;

template <class resource_type, class resource_manager_type>
requires (!std::is_same_v<resource_manager_type, void>)
&& concepts::loadable_with_manager_resource<resource_type, resource_manager_type>
inline constexpr bool is_loadable_resource_v<resource_type, resource_manager_type> = true;

template <class resource_type>
requires concepts::loadable_resource<resource_type>
inline constexpr bool is_loadable_resource_v<resource_type, void> = true;
}

}
}
