#include <arba/rsce/resource_store.hpp>
#include <arba/rsce/basic_resource_manager.hpp>
#include <gtest/gtest.h>
#include "resources/text.hpp"
#include "resources/text_mngr.hpp"
#include "resources/story.hpp"
#include "resources/story_mngr.hpp"
#include "resources/resources_helper.hpp"

static_assert(rsce::traits::is_loadable_resource_v<text>);
static_assert(rsce::traits::is_loadable_resource_v<text_mngr, rsce::basic_resource_manager>);

using text_sptr = rsce::resource_store<text>::resource_sptr;
using text_mngr_sptr = rsce::resource_store<text_mngr>::resource_sptr;

namespace rsce
{
template class default_resource_store<text>;
}

// Unit tests:

TEST(resource_store_tests, constructor__no_arg__no_error)
{
    rsce::resource_store<text> text_store;
    ASSERT_EQ(text_store.size(), 0);
}

TEST(resource_store_tests, get_shared__resource_file_exists__no_exception)
{
    std::filesystem::path rsc = rscdir();

    try
    {
        rsce::resource_store<text> text_store;
        text_sptr koro_sptr = text_store.get_shared(rsc/"koro.txt");
        text_sptr koro_sptr_2 = text_store.get_shared(rsc/"koro.txt");
        ASSERT_NE(koro_sptr, nullptr);
        ASSERT_EQ(koro_sptr, koro_sptr_2);
        ASSERT_EQ(koro_sptr->contents, koro_contents());

        ASSERT_EQ(text_store.size(), 1);
    }
    catch (...)
    {
        FAIL();
    }
}

TEST(resource_store_tests, get_shared__invalid_resource_file_exists__no_exception)
{
    std::filesystem::path rsc = rscdir();

    rsce::resource_store<story> story_store;
    try
    {
        std::shared_ptr koro_sptr = story_store.get_shared(rsc/"koro.txt");
    }
    catch (...)
    {
        ASSERT_EQ(story_store.size(), 0);
    }
}

TEST(resource_store_tests, get_shared__resource_file_does_not_exist__not_found_exception)
{
    std::filesystem::path rsc = rscdir();

    rsce::resource_store<text> text_store;
    try
    {
        text_sptr not_found_sptr = text_store.get_shared(rsc/"not_found.txt");
        FAIL();
    }
    catch (...)
    {
        ASSERT_EQ(text_store.size(), 0);
    }
}

TEST(resource_store_tests, get_shared__resource_mngr_file_exists__no_exception)
{
    std::filesystem::path rsc = rscdir();
    rsce::basic_resource_manager rmanager;

    try
    {
        rsce::resource_store<text_mngr> text_store;
        text_mngr_sptr koro_sptr = text_store.get_shared(rsc/"koro.txt", rmanager);
        text_mngr_sptr koro_sptr_2 = text_store.get_shared(rsc/"koro.txt", rmanager);
        ASSERT_NE(koro_sptr, nullptr);
        ASSERT_EQ(koro_sptr, koro_sptr_2);
        ASSERT_EQ(koro_sptr->contents, koro_contents());

        ASSERT_EQ(text_store.size(), 1);
    }
    catch (...)
    {
        FAIL();
    }
}

TEST(resource_store_tests, get_shared__invalid_resource_mngr_file_exists__no_exception)
{
    std::filesystem::path rsc = rscdir();
    rsce::basic_resource_manager rmanager;

    rsce::resource_store<story_mngr> story_store;
    try
    {
        std::shared_ptr koro_sptr = story_store.get_shared(rsc/"koro.txt", rmanager);
    }
    catch (...)
    {
        ASSERT_EQ(story_store.size(), 0);
    }
}

TEST(resource_store_tests, get_shared__resource_mngr_file_does_not_exist__not_found_exception)
{
    std::filesystem::path rsc = rscdir();
    rsce::basic_resource_manager rmanager;

    rsce::resource_store<text_mngr> text_store;
    try
    {
        text_mngr_sptr not_found_sptr = text_store.get_shared(rsc/"not_found.txt", rmanager);
        FAIL();
    }
    catch (...)
    {
        ASSERT_EQ(text_store.size(), 0);
    }
}

TEST(resource_store_tests, load__resource_file_exists__expect_no_exception)
{
    std::filesystem::path rsc = rscdir();
    rsce::resource_store<text> text_store;

    try
    {
        text_sptr koro_sptr = text_store.load(rsc/"koro.txt");
        text_sptr tiki_sptr = text_store.load(rsc/"tiki.txt");
        ASSERT_EQ(text_store.size(), 2);
        text_sptr koro_2_sptr = text_store.load(rsc/"koro.txt");
        ASSERT_EQ(text_store.size(), 2);
        ASSERT_NE(koro_sptr, koro_2_sptr);
        ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
    }
    catch (...)
    {
        FAIL();
    }
}

TEST(resource_store_tests, load__resource_file_does_not_exist__not_found_exception)
{
    std::filesystem::path rsc = rscdir();
    rsce::resource_store<text> text_store;

    try
    {
        text_sptr koro_sptr = text_store.load(rsc/"not_found.txt");
        FAIL();
    }
    catch (...)
    {
        ASSERT_EQ(text_store.size(), 0);
    }
}

TEST(resource_store_tests, load__resource_mngr_file_exists__expect_no_exception)
{
    std::filesystem::path rsc = rscdir();
    rsce::basic_resource_manager rmanager;
    rsce::resource_store<text_mngr> text_store;

    try
    {
        text_mngr_sptr koro_sptr = text_store.load(rsc/"koro.txt", rmanager);
        text_mngr_sptr tiki_sptr = text_store.load(rsc/"tiki.txt", rmanager);
        ASSERT_EQ(text_store.size(), 2);
        text_mngr_sptr koro_2_sptr = text_store.load(rsc/"koro.txt", rmanager);
        ASSERT_EQ(text_store.size(), 2);
        ASSERT_NE(koro_sptr, koro_2_sptr);
        ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
    }
    catch (...)
    {
        FAIL();
    }
}

TEST(resource_store_tests, load__resource_mngr_file_does_not_exist__not_found_exception)
{
    std::filesystem::path rsc = rscdir();
    rsce::basic_resource_manager rmanager;
    rsce::resource_store<text_mngr> text_store;

    try
    {
        text_mngr_sptr koro_sptr = text_store.load(rsc/"not_found.txt", rmanager);
        FAIL();
    }
    catch (...)
    {
        ASSERT_EQ(text_store.size(), 0);
    }
}

TEST(resource_store_tests, clear__no_arg__no_error)
{
    std::filesystem::path rsc = rscdir();

    rsce::resource_store<text> text_store;
    text_store.get_shared(rsc/"koro.txt");
    text_store.get_shared(rsc/"tiki.txt");
    ASSERT_EQ(text_store.size(), 2);
    text_store.clear();
    ASSERT_EQ(text_store.size(), 0);
}

TEST(resource_store_tests, insert__rsc_sptr__no_error)
{
    rsce::resource_store<text> text_store;
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    bool ins_res = text_store.insert("default_tale", tale_sptr);
    ASSERT_TRUE(ins_res);
    ASSERT_EQ(tale_sptr, text_store.get_shared("default_tale"));
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    ins_res = text_store.insert("default_tale", tale_2_sptr);
    ASSERT_FALSE(ins_res);
    ASSERT_EQ(tale_sptr, text_store.get_shared("default_tale"));
}

TEST(resource_store_tests, set__rsc_sptr__no_error)
{
    rsce::resource_store<text> text_store;
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    text* tale_ptr = tale_sptr.get();
    text_store.set("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, text_store.get_shared("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    text* tale_2_ptr = tale_2_sptr.get();
    text_store.set("default_tale", std::move(tale_2_sptr));
    ASSERT_EQ(tale_2_ptr, text_store.get_shared("default_tale").get());
    ASSERT_EQ(tale_2_ptr->contents, "Once upon a time 2");
}

TEST(resource_store_tests, remove__rsc_path__no_error)
{
    std::filesystem::path rsc = rscdir();

    rsce::resource_store<text> text_store;
    text_sptr koro_sptr = text_store.load(rsc/"koro.txt");
    text_sptr tiki_sptr = text_store.load(rsc/"tiki.txt");
    ASSERT_EQ(text_store.size(), 2);
    text_store.remove(rsc/"koro.txt");
    ASSERT_EQ(text_store.size(), 1);
    text_store.remove(rsc/"tiki.txt");
    ASSERT_EQ(text_store.size(), 0);
}

int main(int argc, char** argv)
{
    create_resource_files();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
