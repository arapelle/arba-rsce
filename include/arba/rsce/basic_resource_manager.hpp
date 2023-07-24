#pragma once

#include "resource_store.hpp"
#include <typeinfo>
#include <atomic>
#include <cassert>

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
    inline resource& get(const std::filesystem::path& rsc_path)
    {
        std::shared_ptr<resource> rsc_sptr = get_shared<resource>(rsc_path);
        assert(rsc_sptr);
        return *rsc_sptr;
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
    inline void set(const std::filesystem::path& rsc_path, resource&& rsc)
    {
        get_or_create_resource_store_<resource>().set(rsc_path, std::move(rsc));
    }

    template <class resource>
    inline std::shared_ptr<resource> load(const std::filesystem::path& rsc_path)
    {
        return get_or_create_resource_store_<resource>().load(rsc_path, *this);
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
        std::lock_guard lock(mutex_);
        std::size_t rsc_type_index = resource_type_index_<resource>();
        if (rsc_type_index)
        {
            std::ostringstream stream("No resource of this type is stored in this manager. Resource: ");
            stream << typeid(resource).name();
            throw std::invalid_argument(stream.str());
        }
        return *static_cast<resource_store<resource>*>(resource_stores_[rsc_type_index].get());
    }

    template <class resource>
    inline resource_store<resource>& get_or_create_resource_store_()
    {
        std::lock_guard lock(mutex_);

        std::size_t index = resource_type_index_<resource>();
        if (index >= resource_stores_.size())
            resource_stores_.resize(index + 1);

        resource_store_interface_uptr& resource_store_uptr = resource_stores_[index];
        if (!resource_store_uptr)
        {
            resource_store_interface_uptr n_event = std::make_unique<resource_store<resource>>();
            resource_store_uptr = std::move(n_event);
        }

        return *static_cast<resource_store<resource>*>(resource_store_uptr.get());
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
    std::recursive_mutex mutex_;
};

}
}
