#pragma once

#include <filesystem>
#include <memory>

namespace rsce
{
template <class resource_type>
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path);

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

class resource_manager;

template <class resource_type>
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path, resource_manager& rsc_manager);

template <class resource_type>
requires requires(resource_type& value, const std::filesystem::path& fpath, resource_manager& rsc_manager)
{
    { value.load_from_file(fpath, rsc_manager) } -> std::convertible_to<bool>;
}
std::shared_ptr<resource_type> load_resource_from_file(const std::filesystem::path& path, resource_manager& rsc_manager)
{
    resource_type value;
    if (value.load_from_file(path))
        return std::make_shared<resource_type>(std::move(value));
    return std::shared_ptr<resource_type>();
}

}
