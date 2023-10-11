#include <gtest/gtest.h>
#include <arba/rsce/basic_resource_manager.hpp>
#include <arba/core/plugin.hpp>
#include <rng_interface.hpp>
#include <format>
#include <regex>

std::filesystem::path plugin_dir = PLUGIN_DIR;

TEST(plugin_as_resource, LoadFromFile_ExistingLibraryWithExtension_ExpectNoException)
{
    try
    {
        rsce::basic_resource_manager rsc_manager;
        const std::filesystem::path plugin_path = plugin_dir / std::format("librsce_rng{}", core::plugin::file_extension);
        std::shared_ptr plugin_sptr = rsc_manager.get_shared<core::plugin>(plugin_path);
        ASSERT_TRUE(plugin_sptr->is_loaded());

        std::unique_ptr generator = plugin_sptr->make_unique_instance<generator_interface>();
        int value = generator->generate();
        ASSERT_GT(value, 0);
    }
    catch(...)
    {
        throw;
    }
}

TEST(plugin_as_resource, LoadFromFile_UnfoundLibrary_ExpectException)
{
    rsce::basic_resource_manager rsc_manager;

    try
    {
        const std::filesystem::path plugin_path = plugin_dir / std::format("notfound.{}", core::plugin::file_extension);
        std::shared_ptr plugin_sptr = rsc_manager.get_shared<core::plugin>(plugin_path);
        FAIL();
    }
    catch(...)
    {
        ASSERT_EQ(rsc_manager.number_of_resources<core::plugin>(), 0);
    }
}
