#pragma once

#include "basic_resource_manager.hpp"
#include <arba/vlfs/vlfs.hpp>

inline namespace arba
{
namespace rsce
{

class resource_manager : public basic_resource_manager
{
public:
    explicit resource_manager(vlfs::virtual_filesystem& vlfs) : vlfs_(&vlfs) {}

    inline const vlfs::virtual_filesystem& virtual_filesystem() const { return *vlfs_; }
    inline vlfs::virtual_filesystem& virtual_filesystem() { return *vlfs_; }

    template <class resource>
    inline std::shared_ptr<resource> get_shared(const std::filesystem::path& rsc_path)
    {
        if (vlfs::virtual_filesystem::path_components path_comps = vlfs_->extract_components(rsc_path);
            path_comps)
        {
            return basic_resource_manager::get_shared<resource>(vlfs_->real_path(path_comps));
        }
        return this->get_or_create_resource_store_<resource>().get_shared(rsc_path, *this);
    }

    template <class resource>
    inline std::shared_ptr<resource> get_shared(std::filesystem::path&& rsc_path)
    {
        std::filesystem::path real_path(std::move(rsc_path));
        vlfs_->convert_to_real_path(real_path);
        return this->get_or_create_resource_store_<resource>().get_shared(real_path, *this);
    }

    template <class resource>
    inline std::shared_ptr<resource> get_shared(const std::filesystem::path& rsc_path, std::nothrow_t)
    {
        if (vlfs::virtual_filesystem::path_components path_comps = vlfs_->extract_components(rsc_path);
            path_comps)
        {
            return basic_resource_manager::get_shared<resource>(vlfs_->real_path(path_comps), std::nothrow);
        }
        return basic_resource_manager::get_shared<resource>(rsc_path, std::nothrow);
    }

    template <class resource>
    inline std::shared_ptr<resource> get_shared(std::filesystem::path&& rsc_path, std::nothrow_t)
    {
        std::filesystem::path real_path(std::move(rsc_path));
        vlfs_->convert_to_real_path(real_path);
        return basic_resource_manager::get_shared<resource>(real_path, std::nothrow);
    }

    template <class resource>
    inline resource& get(const std::filesystem::path& rsc_path)
    {
        std::shared_ptr<resource> rsc_sptr = get_shared<resource>(rsc_path);
        return *rsc_sptr;
    }

    template <class resource>
    inline resource& get(std::filesystem::path&& rsc_path)
    {
        std::shared_ptr<resource> rsc_sptr = get_shared<resource>(std::move(rsc_path));
        return *rsc_sptr;
    }

    template <class resource>
    inline bool insert(const std::filesystem::path& rsc_path, std::shared_ptr<resource> rsc_sptr)
    {
        if (vlfs::virtual_filesystem::path_components path_comps = vlfs_->extract_components(rsc_path);
            path_comps)
        {
            return this->basic_resource_manager::insert<resource>(vlfs_->real_path(path_comps), rsc_sptr);
        }
        return this->basic_resource_manager::insert<resource>(rsc_path, rsc_sptr);
    }

    template <class resource>
    inline bool insert(std::filesystem::path&& rsc_path, std::shared_ptr<resource> rsc_sptr)
    {
        std::filesystem::path real_path(std::move(rsc_path));
        vlfs_->convert_to_real_path(real_path);
        return this->basic_resource_manager::insert<resource>(real_path, rsc_sptr);
    }

    template <class resource>
    inline void set(const std::filesystem::path& rsc_path, std::shared_ptr<resource> rsc_sptr)
    {
        if (vlfs::virtual_filesystem::path_components path_comps = vlfs_->extract_components(rsc_path);
            path_comps)
        {
            this->basic_resource_manager::set<resource>(vlfs_->real_path(path_comps), std::move(rsc_sptr));
        }
        else
            this->basic_resource_manager::set<resource>(rsc_path, std::move(rsc_sptr));
    }

    template <class resource>
    inline void set(std::filesystem::path&& rsc_path, std::shared_ptr<resource> rsc_sptr)
    {
        std::filesystem::path real_path(std::move(rsc_path));
        vlfs_->convert_to_real_path(real_path);
        this->basic_resource_manager::set<resource>(real_path, std::move(rsc_sptr));
    }

    template <class resource>
    inline std::shared_ptr<resource> load(const std::filesystem::path& rsc_path)
    {
        if (vlfs::virtual_filesystem::path_components path_comps = vlfs_->extract_components(rsc_path);
            path_comps)
        {
            return basic_resource_manager::load<resource>(vlfs_->real_path(path_comps));
        }
        return basic_resource_manager::load<resource>(rsc_path);
    }

    template <class resource>
    inline std::shared_ptr<resource> load(std::filesystem::path&& rsc_path)
    {
        std::filesystem::path real_path(std::move(rsc_path));
        vlfs_->convert_to_real_path(real_path);
        return basic_resource_manager::load<resource>(real_path);
    }

    template <class resource>
    inline std::shared_ptr<resource> load(const std::filesystem::path& rsc_path, std::nothrow_t)
    {
        if (vlfs::virtual_filesystem::path_components path_comps = vlfs_->extract_components(rsc_path);
            path_comps)
        {
            return basic_resource_manager::load<resource>(vlfs_->real_path(path_comps), std::nothrow);
        }
        return basic_resource_manager::load<resource>(rsc_path, std::nothrow);
    }

    template <class resource>
    inline std::shared_ptr<resource> load(std::filesystem::path&& rsc_path, std::nothrow_t)
    {
        std::filesystem::path real_path(std::move(rsc_path));
        vlfs_->convert_to_real_path(real_path);
        return basic_resource_manager::load<resource>(real_path, std::nothrow);
    }

    template <class resource>
    inline void remove(const std::filesystem::path& rsc_path)
    {
        if (vlfs::virtual_filesystem::path_components path_comps = vlfs_->extract_components(rsc_path);
            path_comps)
        {
            this->basic_resource_manager::remove<resource>(vlfs_->real_path(path_comps));
        }
        else
            this->basic_resource_manager::remove<resource>(rsc_path);
    }

    template <class resource>
    inline void remove(std::filesystem::path&& rsc_path)
    {
        std::filesystem::path real_path(std::move(rsc_path));
        vlfs_->convert_to_real_path(real_path);
        this->basic_resource_manager::remove<resource>(real_path);
    }

private:
    vlfs::virtual_filesystem* vlfs_ = nullptr;
};

}
}
