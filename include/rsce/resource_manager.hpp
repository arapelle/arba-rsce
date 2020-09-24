#pragma once

#include "basic_resource_manager.hpp"
#include <vlfs/vlfs.hpp>

namespace rsce
{
class resource_manager : public basic_resource_manager
{
public:
    explicit resource_manager(vlfs::virtual_filesystem& vlfs) : vlfs_(&vlfs) {}

    inline const vlfs::virtual_filesystem* virtual_filesystem() const { return vlfs_; }
    inline vlfs::virtual_filesystem* virtual_filesystem() { return vlfs_; }

    template <class resource>
    inline std::shared_ptr<resource> get(const std::filesystem::path& rsc_path)
    {
        return this->get_or_create_resource_store_<resource>().get(vlfs_->real_path(rsc_path), *this);
    }

    template <class resource>
    inline resource& get_ref(const std::filesystem::path& rsc_path)
    {
        std::shared_ptr<resource> rsc_sptr = get<resource>(rsc_path);
        assert(rsc_sptr);
        return *rsc_sptr;
    }

    template <class resource>
    inline bool insert(const std::filesystem::path& rsc_path, std::shared_ptr<resource> rsc_sptr)
    {
        return this->basic_resource_manager::insert<resource>(vlfs_->real_path(rsc_path), rsc_sptr);
    }

    template <class resource>
    inline void set(const std::filesystem::path& rsc_path, std::shared_ptr<resource>&& rsc_sptr)
    {
        this->basic_resource_manager::set<resource>(vlfs_->real_path(rsc_path), std::move(rsc_sptr));
    }

    template <class resource>
    inline void set(const std::filesystem::path& rsc_path, resource&& rsc)
    {
        this->basic_resource_manager::set<resource>(vlfs_->real_path(rsc_path), std::move(rsc));
    }

    template <class resource>
    inline std::shared_ptr<resource> load(const std::filesystem::path& rsc_path)
    {
        return this->get_or_create_resource_store_<resource>().load(vlfs_->real_path(rsc_path), *this);
    }

    template <class resource>
    inline void remove(const std::filesystem::path& rsc_path)
    {
        this->basic_resource_manager::remove<resource>(vlfs_->real_path(rsc_path));
    }

private:
    vlfs::virtual_filesystem* vlfs_ = nullptr;
};
}
