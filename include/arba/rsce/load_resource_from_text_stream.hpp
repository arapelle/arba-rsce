#pragma once

#include <istream>
#include <memory>

inline namespace arba
{
namespace rsce
{

template <class resource_type>
std::shared_ptr<resource_type> load_resource_from_text_stream(std::istream& stream) = delete;

template <class resource_type>
requires requires(resource_type& value, std::istream& stream)
{
    { value.load_from_text_stream(stream) } -> std::convertible_to<bool>;
}
std::shared_ptr<resource_type> load_resource_from_text_stream(std::istream& stream)
{
    if (std::shared_ptr rsc_sptr = std::make_shared<resource_type>();
        rsc_sptr->load_from_text_stream(stream)) [[likely]]
        return rsc_sptr;
    return std::shared_ptr<resource_type>();
}

template <class resource_type>
    requires requires(resource_type& value, std::istream& stream)
{
    { value.load_from_text_stream(stream) } -> std::same_as<void>;
}
std::shared_ptr<resource_type> load_resource_from_text_stream(std::istream& stream)
{
    std::shared_ptr rsc_sptr = std::make_shared<resource_type>();
    rsc_sptr->load_from_text_stream(stream);
    return rsc_sptr;
}

template <class resource_type, class resource_manager_type>
std::shared_ptr<resource_type> load_resource_from_text_stream(std::istream& stream, resource_manager_type& rsc_manager) = delete;

template <class resource_type, class resource_manager_type>
requires requires(resource_type& value, std::istream& stream, resource_manager_type& rsc_manager)
{
    { value.load_from_text_stream(stream, rsc_manager) } -> std::convertible_to<bool>;
}
std::shared_ptr<resource_type> load_resource_from_text_stream(std::istream& stream, resource_manager_type &rsc_manager)
{
    if (std::shared_ptr rsc_sptr = std::make_shared<resource_type>();
        rsc_sptr->load_from_text_stream(stream, rsc_manager)) [[likely]]
        return rsc_sptr;
    return std::shared_ptr<resource_type>();
}

template <class resource_type, class resource_manager_type>
    requires requires(resource_type& value, std::istream& stream, resource_manager_type& rsc_manager)
{
    { value.load_from_text_stream(stream, rsc_manager) } -> std::same_as<void>;
}
std::shared_ptr<resource_type> load_resource_from_text_stream(std::istream& stream, resource_manager_type &rsc_manager)
{
    std::shared_ptr rsc_sptr = std::make_shared<resource_type>();
    rsc_sptr->load_from_text_stream(stream, rsc_manager);
    return rsc_sptr;
}

}
}
