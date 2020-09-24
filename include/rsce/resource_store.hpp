#pragma once

#include "load_resource_from_file.hpp"
#include <unordered_map>
#include <filesystem>
#include <functional>
#include <cassert>
#include <sstream>
#include <memory>
#include <mutex>
#include <concepts>

namespace rsce
{
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

    resource_sptr get(const std::filesystem::path& rsc_path);
    bool          insert(const std::filesystem::path& rsc_path, resource_sptr resource);
    void          set(const std::filesystem::path& rsc_path, resource_sptr &&resource);
    void          set(const std::filesystem::path& rsc_path, resource_type&& resource);
    resource_sptr load(const std::filesystem::path& rsc_path);
    inline void   remove(const std::filesystem::path& rsc_path) { resources_.erase(rsc_path); }

private:
    resource_dico resources_;
    std::recursive_mutex mutex_;
};

// Template methods implementation:

template <class resource_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::get(const std::filesystem::path &rsc_path)
{
    std::lock_guard lock(mutex_);
    auto iter = resources_.find(rsc_path);
    if (iter != resources_.end())
        return iter->second;
    return load(rsc_path);
}

template <class resource_type>
bool default_resource_store<resource_type>::insert(const std::filesystem::path &rsc_path, resource_sptr resource)
{
    assert(resource);
    std::lock_guard lock(mutex_);
    return resources_.insert(typename resource_dico::value_type(rsc_path, std::move(resource))).second;
}

template <class resource_type>
void default_resource_store<resource_type>::set(const std::filesystem::path &rsc_path, resource_sptr &&resource)
{
    std::lock_guard lock(mutex_);
    resources_[rsc_path] = std::move(resource);
}

template <class resource_type>
void default_resource_store<resource_type>::set(const std::filesystem::path &rsc_path, resource_type &&resource)
{
    std::lock_guard lock(mutex_);
    resource_sptr& rsc_sptr = resources_[rsc_path];
    if (rsc_sptr)
        *rsc_sptr = std::move(resource);
    else
        rsc_sptr = std::make_shared<resource_type>(std::move(resource));
}

template <class resource_type>
default_resource_store<resource_type>::resource_sptr
default_resource_store<resource_type>::load(const std::filesystem::path &rsc_path)
{
    if (std::filesystem::exists(rsc_path))
    {
//        std::cout << "loading the resource file " << rsc_path << ".";
        if (resource_sptr resource = load_resource_from_file<resource_type>(rsc_path); resource)
        {
            std::lock_guard lock(mutex_);
            resources_.emplace(rsc_path, resource);
            return resource;
        }
        else
        {
            std::ostringstream stream;
            stream << "The resource file " << rsc_path << " wasn't loaded correctly (nullptr returned).";
            std::runtime_error(stream.str());
        }
    }
    else
    {
        std::ostringstream stream;
        stream << "The resource file " << rsc_path << " does not exist.";
        std::runtime_error(stream.str());
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
