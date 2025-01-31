#pragma once

#include "resource_store.hpp"

#include <atomic>
#include <shared_mutex>
#include <typeinfo>

inline namespace arba
{
namespace rsce
{

class basic_resource_manager
{
public:
    basic_resource_manager() = default;
    basic_resource_manager(const basic_resource_manager&) = delete;
    basic_resource_manager& operator=(const basic_resource_manager&) = delete;

    template <class resource>
    inline std::shared_ptr<resource> get_shared(const std::filesystem::path& rsc_path)
    {
        return get_or_create_resource_store_<resource>().get_shared(rsc_path, *this);
    }

    template <class resource>
    inline std::shared_ptr<resource> get_shared(const std::filesystem::path& rsc_path, std::nothrow_t)
    {
        try
        {
            return get_or_create_resource_store_<resource>().get_shared(rsc_path, *this);
        }
        catch (const std::exception&)
        {
        }
        return std::shared_ptr<resource>();
    }

    template <class resource>
    inline resource& get(const std::filesystem::path& rsc_path)
    {
        return *get_shared<resource>(rsc_path);
    }

    template <class resource>
    inline bool insert(const std::filesystem::path& rsc_path, std::shared_ptr<resource> rsc_sptr)
    {
        return get_or_create_resource_store_<resource>().insert(rsc_path, rsc_sptr);
    }

    template <class resource>
    inline void set(const std::filesystem::path& rsc_path, std::shared_ptr<resource>&& rsc_sptr)
    {
        get_or_create_resource_store_<resource>().set(rsc_path, std::move(rsc_sptr));
    }

    template <class resource>
    inline std::shared_ptr<resource> load(const std::filesystem::path& rsc_path)
    {
        return get_or_create_resource_store_<resource>().load(rsc_path, *this);
    }

    template <class resource>
    inline std::shared_ptr<resource> load(const std::filesystem::path& rsc_path, std::nothrow_t)
    {
        try
        {
            return get_or_create_resource_store_<resource>().load(rsc_path, *this);
        }
        catch (const std::exception&)
        {
        }
        return std::shared_ptr<resource>();
    }

    template <class resource>
    inline void remove(const std::filesystem::path& rsc_path)
    {
        get_store_<resource>().remove(rsc_path);
    }

    template <class resource>
    inline std::size_t number_of_resources()
    {
        return get_or_create_resource_store_<resource>().size();
    }

    template <class resource>
    inline resource_store<resource>& store()
    {
        return get_or_create_resource_store_<resource>();
    }

protected:
    template <class resource>
    inline resource_store<resource>& get_store_()
    {
        std::size_t rsc_type_index = resource_type_index_<resource>();
        std::shared_lock lock(mutex_);
        if (rsc_type_index >= resource_stores_.size()) [[unlikely]]
            throw_resource_store_is_missing_<resource>();
        resource_store_base* resource_store_ptr = resource_stores_[rsc_type_index].get();
        if (!resource_store_ptr) [[unlikely]]
            throw_resource_store_is_missing_<resource>();
        return *static_cast<resource_store<resource>*>(resource_store_ptr);
    }

    template <class resource>
    inline void throw_resource_store_is_missing_()
    {
        std::string err_str =
            std::format("No resource of this type is stored in this manager. Resource: {}", typeid(resource).name());
        throw std::invalid_argument(err_str);
    }

    template <class resource>
    inline resource_store<resource>& get_or_create_resource_store_()
    {
        std::size_t rsc_type_index = resource_type_index_<resource>();

        resource_store_base* resource_store_ptr = nullptr;
        {
            std::shared_lock lock(mutex_);
            if (rsc_type_index < resource_stores_.size()) [[likely]]
                resource_store_ptr = resource_stores_[rsc_type_index].get();
        }

        if (!resource_store_ptr) [[unlikely]]
        {
            std::unique_lock lock(mutex_);
            std::size_t min_required_size = rsc_type_index + 1;
            if (min_required_size > resource_stores_.size())
                resource_stores_.resize(min_required_size);
            std::unique_ptr rsc_store_uptr = std::make_unique<resource_store<resource>>();
            resource_store_ptr = rsc_store_uptr.get();
            resource_stores_[rsc_type_index] = std::move(rsc_store_uptr);
        }

        return *static_cast<resource_store<resource>*>(resource_store_ptr);
    }

    inline static std::size_t generate_resource_type_index_()
    {
        static std::atomic_size_t index = 0;
        return index++;
    }

    template <class resource>
    inline static std::size_t resource_type_index_()
    {
        static const std::size_t index = generate_resource_type_index_();
        return index;
    }

    using resource_store_interface_uptr = std::unique_ptr<resource_store_base>;

private:
    std::vector<resource_store_interface_uptr> resource_stores_;
    mutable std::shared_mutex mutex_;
};

} // namespace rsce
} // namespace arba
