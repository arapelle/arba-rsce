#include "resources/resources_helper.hpp"
#include "resources/text.hpp"
#include <arba/rsce/basic_resource_manager.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <string>

using text_sptr = rsce::resource_store<text>::resource_sptr;

class red_text : public text
{
};

class green_text : public text
{
};

// Unit tests:

TEST(basic_resource_manager_tests, constructor__no_arg__no_error)
{
    arba::rsce::basic_resource_manager rmanager;
}

TEST(basic_resource_manager_tests, get_shared__resource_file_exists__no_exception)
{
    std::filesystem::path rsc = rscdir();

    try
    {
        rsce::basic_resource_manager rmanager;
        text_sptr koro_sptr = rmanager.get_shared<text>(rsc / "koro.txt");
        text_sptr koro_sptr_2 = rmanager.get_shared<text>(rsc / "koro.txt");
        ASSERT_NE(koro_sptr, nullptr);
        ASSERT_EQ(koro_sptr, koro_sptr_2);
        ASSERT_EQ(koro_sptr->contents, koro_contents());

        ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
    }
    catch (...)
    {
        FAIL();
    }
}

TEST(basic_resource_manager_tests, get_shared__resource_file_does_not_exist__not_found_exception)
{
    std::filesystem::path rsc = rscdir();

    try
    {
        rsce::basic_resource_manager rmanager;
        text_sptr rsc_sptr = rmanager.get_shared<text>(rsc / "not_found.txt");
        FAIL();
    }
    catch (const std::runtime_error& err)
    {
        // err.what() == ...
        SUCCEED();
    }
}

TEST(basic_resource_manager_tests, get_shared__resource_file_does_not_exist_nothrow__no_exception)
{
    std::filesystem::path rsc = rscdir();

    try
    {
        rsce::basic_resource_manager rmanager;
        text_sptr rsc_sptr = rmanager.get_shared<text>(rsc / "not_found.txt", std::nothrow);
        ASSERT_EQ(rsc_sptr, nullptr);
    }
    catch (...)
    {
        FAIL();
    }
}

TEST(basic_resource_manager_tests, load__resource_file_exists__expect_no_exception)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    try
    {
        text_sptr koro_sptr = rmanager.load<text>(rsc / "koro.txt");
        text_sptr tiki_sptr = rmanager.load<text>(rsc / "tiki.txt");
        ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
        text_sptr koro_2_sptr = rmanager.load<text>(rsc / "koro.txt");
        ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
        ASSERT_NE(koro_sptr, koro_2_sptr);
        ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
    }
    catch (...)
    {
        FAIL();
    }
}

TEST(basic_resource_manager_tests, load__resource_file_does_not_exist__not_found_exception)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    try
    {
        text_sptr koro_sptr = rmanager.load<text>(rsc / "not_found.txt");
        FAIL();
    }
    catch (...)
    {
        ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
    }
}

TEST(basic_resource_manager_tests, load__resource_file_does_not_exist__no_exception)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    try
    {
        text_sptr koro_sptr = rmanager.load<text>(rsc / "not_found.txt", std::nothrow);
        ASSERT_EQ(koro_sptr, nullptr);
        ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
    }
    catch (...)
    {
        FAIL();
    }
}

TEST(basic_resource_manager_tests, get__resource_file_exists__no_exception)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    text& koro_ref = rmanager.get<text>(rsc / "koro.txt");
    text& koro_ref2 = rmanager.get<text>(rsc / "koro.txt");
    rmanager.get<text>(rsc / "tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_EQ(&koro_ref, &koro_ref2);
}

TEST(basic_resource_manager_tests, insert__rsc_sptr__no_error)
{
    rsce::basic_resource_manager rmanager;
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    bool ins_res = rmanager.insert("default_tale", tale_sptr);
    ASSERT_TRUE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text>("default_tale"));
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    ins_res = rmanager.insert("default_tale", tale_2_sptr);
    ASSERT_FALSE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text>("default_tale"));
}

TEST(basic_resource_manager_tests, set__rsc_sptr__no_error)
{
    rsce::basic_resource_manager rmanager;
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    text* tale_ptr = tale_sptr.get();
    rmanager.set<text>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get_shared<text>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    text* tale_2_ptr = tale_2_sptr.get();
    rmanager.set<text>("default_tale", std::move(tale_2_sptr));
    ASSERT_EQ(tale_2_ptr, rmanager.get_shared<text>("default_tale").get());
    ASSERT_EQ(tale_2_ptr->contents, "Once upon a time 2");
}

TEST(basic_resource_manager_tests, remove__rsc_path__no_error)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    text_sptr koro_sptr = rmanager.load<text>(rsc / "koro.txt");
    text_sptr tiki_sptr = rmanager.load<text>(rsc / "tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    rmanager.remove<text>(rsc / "koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
    rmanager.remove<text>(rsc / "tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
    rmanager.remove<text>(rsc / "tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 0);

    rmanager.load<red_text>(rsc / "tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<red_text>(), 1);
    rmanager.remove<red_text>(rsc / "tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
}

TEST(basic_resource_manager_tests, remove__rsc_path_missing_resource_store__invalid_arg_exception)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    text_sptr koro_sptr = rmanager.load<red_text>(rsc / "koro.txt");
    rmanager.load<green_text>(rsc / "tiki.txt");
    try
    {
        rmanager.remove<green_text>(rsc / "tiki.txt");
    }
    catch (const std::invalid_argument& ex)
    {
        ASSERT_EQ(rmanager.number_of_resources<red_text>(), 1);
    }
}

TEST(basic_resource_manager_tests, store__no_arg__no_error)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    text_sptr tiki_sptr = rmanager.load<text>(rsc / "tiki.txt");
    rsce::resource_store<text>& text_store = rmanager.store<text>();
    text_sptr tiki_sptr_2 = text_store.get_shared(rsc / "tiki.txt");
    ASSERT_EQ(text_store.size(), 1);
    ASSERT_EQ(tiki_sptr, tiki_sptr_2);
}

TEST(basic_resource_manager_tests, test_unordered_store_creation)
{
    std::filesystem::path rsc = rscdir();

    {
        rsce::basic_resource_manager rmanager;
        ASSERT_NE(rmanager.get_shared<red_text>(rsc / "tiki.txt"), nullptr);   // red_text index = 0.
        ASSERT_NE(rmanager.get_shared<green_text>(rsc / "tiki.txt"), nullptr); // green_text index = 1.
    }
    {
        rsce::basic_resource_manager rmanager;
        // No problem to create green_text (index=1) store before red_text store (index=0).
        text_sptr green_tiki_sptr = rmanager.get_shared<green_text>(rsc / "tiki.txt");
        ASSERT_NE(rmanager.load<red_text>(rsc / "tiki.txt"), nullptr);
        text_sptr green_tiki_sptr_2 = rmanager.get_shared<green_text>(rsc / "tiki.txt");
        ASSERT_EQ(green_tiki_sptr, green_tiki_sptr_2);
    }
}

int main(int argc, char** argv)
{
    create_resource_files();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
