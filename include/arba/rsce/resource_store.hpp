#pragma once

#include "load_resource_from_file.hpp"

#include <cassert>
#include <concepts>
#include <filesystem>
#include <format>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

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
    inline void clear() { resources_.clear(); }
    inline void reserve(std::size_t capacity) { resources_.reserve(capacity); }

    template <class resource_manager_type>
    resource_sptr get_shared(const std::filesystem::path& rsc_path, resource_manager_type& rsc_manager);
    resource_sptr get_shared(const std::filesystem::path& rsc_path);

    template <class resource_manager_type>
    inline resource_sptr load(const std::filesystem::path& rsc_path, resource_manager_type& rsc_manager);
    inline resource_sptr load(const std::filesystem::path& rsc_path);

    inline bool insert(const std::filesystem::path& rsc_path, resource_sptr resource);
    inline void set(const std::filesystem::path& rsc_path, resource_sptr resource);
    inline void remove(const std::filesystem::path& rsc_path);

private:
    inline resource_sptr load_canonical_(const std::filesystem::path& rsc_path);
    template <class resource_manager_type>
        requires traits::is_loadable_resource_v<resource_type, resource_manager_type>
    inline resource_sptr load_canonical_(const std::filesystem::path& rsc_path, resource_manager_type& rsc_manager);
    resource_sptr emplace_if_valid_(const std::filesystem::path& rsc_path, resource_sptr rsc_sptr);

private:
    resource_dico resources_;
    std::recursive_mutex mutex_;
};

// Template methods implementation:

template <class resource_type>
template <class resource_manager_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::get_shared(const std::filesystem::path& rsc_path,
                                                  resource_manager_type& rsc_manager)
{
    std::lock_guard lock(mutex_);

    auto iter = resources_.find(rsc_path);
    if (iter != resources_.end())
        return iter->second;

    std::filesystem::path c_rsc_path = std::filesystem::canonical(rsc_path);
    iter = resources_.find(c_rsc_path);
    if (iter != resources_.end())
        return iter->second;

    if constexpr (traits::is_loadable_resource_v<resource_type, resource_manager_type>)
    {
        return load_canonical_(c_rsc_path, rsc_manager);
    }
    else
    {
        return load_canonical_(c_rsc_path);
    }
}

template <class resource_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::get_shared(const std::filesystem::path& rsc_path)
{
    std::lock_guard lock(mutex_);

    auto iter = resources_.find(rsc_path);
    if (iter != resources_.end())
        return iter->second;

    std::filesystem::path c_rsc_path = std::filesystem::canonical(rsc_path);
    iter = resources_.find(c_rsc_path);
    if (iter != resources_.end())
        return iter->second;

    return load_canonical_(c_rsc_path);
}

template <class resource_type>
template <class resource_manager_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::load(const std::filesystem::path& rsc_path, resource_manager_type& rsc_manager)
{
    std::filesystem::path c_rsc_path = std::filesystem::canonical(rsc_path);
    if constexpr (traits::is_loadable_resource_v<resource_type, resource_manager_type>)
    {
        return load_canonical_(c_rsc_path, rsc_manager);
    }
    else
    {
        return load_canonical_(c_rsc_path);
    }
}

template <class resource_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::load(const std::filesystem::path& rsc_path)
{
    std::filesystem::path c_rsc_path = std::filesystem::canonical(rsc_path);
    return load_canonical_(c_rsc_path);
}

template <class resource_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::load_canonical_(const std::filesystem::path& c_rsc_path)
{
    resource_sptr rsc_sptr = load_resource_from_file<resource_type>(c_rsc_path);
    return emplace_if_valid_(c_rsc_path, std::move(rsc_sptr));
}

template <class resource_type>
template <class resource_manager_type>
    requires traits::is_loadable_resource_v<resource_type, resource_manager_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::load_canonical_(const std::filesystem::path& c_rsc_path,
                                                       resource_manager_type& rsc_manager)
{
    resource_sptr rsc_sptr = load_resource_from_file<resource_type>(c_rsc_path, rsc_manager);
    return emplace_if_valid_(c_rsc_path, std::move(rsc_sptr));
}

template <class resource_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::emplace_if_valid_(const std::filesystem::path& c_rsc_path,
                                                         resource_sptr rsc_sptr)
{
    if (rsc_sptr) [[likely]]
    {
        std::lock_guard lock(mutex_);
        resources_.emplace(c_rsc_path, rsc_sptr);
        return rsc_sptr;
    }
    else
    {
        std::string err_str = std::format("The resource file \"{}\" was not loaded correctly (nullptr returned).",
                                          c_rsc_path.generic_string());
        throw std::runtime_error(err_str);
    }
}

template <class resource_type>
bool default_resource_store<resource_type>::insert(const std::filesystem::path& rsc_path, resource_sptr resource)
{
    assert(resource);
    std::lock_guard lock(mutex_);
    return resources_.insert(typename resource_dico::value_type(rsc_path, std::move(resource))).second;
}

template <class resource_type>
void default_resource_store<resource_type>::set(const std::filesystem::path& rsc_path, resource_sptr resource)
{
    assert(resource);
    std::lock_guard lock(mutex_);
    resources_[rsc_path] = std::move(resource);
}

template <class resource_type>
void default_resource_store<resource_type>::remove(const std::filesystem::path& rsc_path)
{
    if (resources_.erase(rsc_path) == 0)
        resources_.erase(std::filesystem::canonical(rsc_path));
}

template <class resource_type>
class resource_store : public default_resource_store<resource_type>
{
public:
    using default_resource_store<resource_type>::default_resource_store;
};

} // namespace rsce
} // namespace arba
