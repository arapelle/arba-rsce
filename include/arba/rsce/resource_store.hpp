#pragma once

#include "load_resource_from_file.hpp"
#include <arba/core/debug/assert.hpp>
#include <arba/core/io/check_file.hpp>
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <format>
#include <memory>
#include <mutex>
#include <concepts>

inline namespace arba
{
namespace rsce
{

class basic_resource_manager;

class resource_store_base
{
public:
    virtual ~resource_store_base() = default;

protected:
    inline resource_store_base() = default;

    struct filesystem_path_hash
    {
        std::size_t operator()(const std::filesystem::path& arg) const noexcept;
    };
};

template <class resource_type>
class default_resource_store : public resource_store_base
{
public:
    using resource = resource_type;
    using resource_sptr = std::shared_ptr<resource>;

private:
    using resource_dico = std::unordered_map<std::filesystem::path, resource_sptr, filesystem_path_hash>;

public:
    default_resource_store() = default;
    virtual ~default_resource_store() override = default;

    inline std::size_t size() { return resources_.size(); }
    inline void        clear() { resources_.clear(); }
    inline void        reserve(std::size_t capacity) { resources_.reserve(capacity); }

    resource_sptr get_shared(const std::filesystem::path& rsc_path);
    template <class resource_manager_type>
    resource_sptr get_shared(const std::filesystem::path& rsc_path, resource_manager_type& rsc_manager);
    bool          insert(const std::filesystem::path& rsc_path, resource_sptr resource);
    void          set(const std::filesystem::path& rsc_path, resource_sptr resource);
    resource_sptr load(const std::filesystem::path& rsc_path);
    inline void   remove(const std::filesystem::path& rsc_path) { resources_.erase(rsc_path); }

    template <class resource_manager_type>
    requires traits::is_loadable_resource_v<resource_type, resource_manager_type>
    resource_sptr load(const std::filesystem::path& rsc_path, resource_manager_type& rsc_manager);

    template <class resource_manager_type>
    requires (!traits::is_loadable_resource_v<resource_type, resource_manager_type>)
    resource_sptr load(const std::filesystem::path& rsc_path, resource_manager_type&) { return load(rsc_path); }

private:
    resource_dico resources_;
    std::recursive_mutex mutex_;
};

// Template methods implementation:

template <class resource_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::get_shared(const std::filesystem::path &rsc_path)
{
    std::lock_guard lock(mutex_);
    auto iter = resources_.find(rsc_path);
    if (iter != resources_.end())
        return iter->second;
    return load(rsc_path);
}

template <class resource_type>
template <class resource_manager_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::get_shared(const std::filesystem::path &rsc_path, resource_manager_type& rsc_manager)
{
    std::lock_guard lock(mutex_);
    auto iter = resources_.find(rsc_path);
    if (iter != resources_.end())
        return iter->second;
    return load(rsc_path, rsc_manager);
}

template <class resource_type>
bool default_resource_store<resource_type>::insert(const std::filesystem::path &rsc_path, resource_sptr resource)
{
    ARBA_ASSERT(resource);
    std::lock_guard lock(mutex_);
    return resources_.insert(typename resource_dico::value_type(rsc_path, std::move(resource))).second;
}

template <class resource_type>
void default_resource_store<resource_type>::set(const std::filesystem::path &rsc_path, resource_sptr resource)
{
    ARBA_ASSERT(resource);
    std::lock_guard lock(mutex_);
    resources_[rsc_path] = std::move(resource);
}

template <class resource_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::load(const std::filesystem::path &rsc_path)
{
    core::check_input_file(rsc_path);

    if (resource_sptr resource = load_resource_from_file<resource_type>(rsc_path); resource) [[likely]]
    {
        std::lock_guard lock(mutex_);
        resources_.emplace(rsc_path, resource);
        return resource;
    }
    else
    {
        std::string err_str = std::format("The resource file \"{}\" wasn't loaded correctly (nullptr returned).",
                                          rsc_path.generic_string());
        throw std::runtime_error(err_str);
    }

    return nullptr;
}

template <class resource_type>
template <class resource_manager_type>
requires traits::is_loadable_resource_v<resource_type, resource_manager_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::load(const std::filesystem::path &rsc_path, resource_manager_type& rsc_manager)
{
    core::check_input_file(rsc_path);

    if (resource_sptr resource = load_resource_from_file<resource_type>(rsc_path, rsc_manager); resource) [[likely]]
    {
        std::lock_guard lock(mutex_);
        resources_.emplace(rsc_path, resource);
        return resource;
    }
    else
    {
        std::string err_str = std::format("The resource file \"{}\" wasn't loaded correctly (nullptr returned).",
                                          rsc_path.generic_string());
        throw std::runtime_error(err_str);
    }

    return nullptr;
}

template <class resource_type>
class resource_store : public default_resource_store<resource_type>
{
public:
    using default_resource_store<resource_type>::default_resource_store;
};

}
}
