#include "resources/resources_helper.hpp"
#include "resources/text.hpp"
#include <arba/rsce/resource_manager.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <string>

using text_sptr = rsce::resource_store<text>::resource_sptr;

// Unit tests:

TEST(resource_manager_tests, constructor__no_arg__no_error)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    ASSERT_EQ(&rmanager.virtual_filesystem(), &vlfs);
}

TEST(resource_manager_tests, get_shared__resource_file_exists__no_exception)
{
    std::filesystem::path rsc = textdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.get_shared<text>(rsc / "koro.txt");
    const std::filesystem::path rsc_path = rsc / "koro.txt";
    text_sptr koro_sptr_2 = rmanager.get_shared<text>(rsc_path);
    ASSERT_NE(koro_sptr, nullptr);
    ASSERT_EQ(koro_sptr, koro_sptr_2);
    ASSERT_EQ(koro_sptr->contents, koro_contents());

    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
}

TEST(resource_manager_tests, get_shared__resource_file_does_not_exist__not_found_exception)
{
    std::filesystem::path rsc = textdir();
    rsce::basic_resource_manager rmanager;

    try
    {
        text_sptr rsc_sptr = rmanager.get_shared<text>(rsc / "not_found.txt");
        FAIL();
    }
    catch (const std::runtime_error&)
    {
        ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
    }
}

TEST(resource_manager_tests, get_shared__resource_file_does_not_exist_lvalue__not_found_exception)
{
    std::filesystem::path rsc = textdir();
    rsce::basic_resource_manager rmanager;

    try
    {
        const std::filesystem::path rsc_path = rsc / "not_found.txt";
        text_sptr rsc_sptr = rmanager.get_shared<text>(rsc_path);
        FAIL();
    }
    catch (const std::runtime_error&)
    {
        ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
    }
}

TEST(resource_manager_tests, get_shared__resource_file_does_not_exist_nothrow__no_exception)
{
    std::filesystem::path rsc = textdir();

    rsce::basic_resource_manager rmanager;
    text_sptr rsc_sptr = rmanager.get_shared<text>(rsc / "not_found.txt", std::nothrow);
    ASSERT_EQ(rsc_sptr, nullptr);
    const std::filesystem::path rsc_path = rsc / "not_found.txt";
    rsc_sptr = rmanager.get_shared<text>(rsc_path, std::nothrow);
    ASSERT_EQ(rsc_sptr, nullptr);
}

TEST(basic_resource_manager_tests, get_shared__resource_file_does_not_exist_nothrow__no_exception)
{
    std::filesystem::path rsc = textdir();

    try
    {
        rsce::basic_resource_manager rmanager;
        text_sptr rsc_sptr = rmanager.get_shared<text>(rsc / "not_found.txt", std::nothrow);
        ASSERT_EQ(rsc_sptr, nullptr);
        const std::filesystem::path rsc_path = rsc / "not_found.txt";
        rsc_sptr = rmanager.get_shared<text>(rsc_path, std::nothrow);
        ASSERT_EQ(rsc_sptr, nullptr);
    }
    catch (...)
    {
        FAIL();
    }
}

TEST(resource_manager_tests, get__resource_file_exists__no_exception)
{
    std::filesystem::path rsc = textdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text& koro_ref = rmanager.get<text>(rsc / "koro.txt");
    text& koro_ref2 = rmanager.get<text>(rsc / "koro.txt");
    rmanager.get<text>(rsc / "tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_EQ(&koro_ref, &koro_ref2);
}

TEST(resource_manager_tests, insert__rsc_sptr__no_error)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    bool ins_res = rmanager.insert("default_tale", tale_sptr);
    ASSERT_TRUE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text>("default_tale"));
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    ins_res = rmanager.insert("default_tale", tale_2_sptr);
    ASSERT_FALSE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text>("default_tale"));
}

TEST(resource_manager_tests, set__rsc_sptr__no_error)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
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

TEST(resource_manager_tests, load__resource_file_exists__expect_no_exception)
{
    std::filesystem::path rsc = textdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.load<text>(rsc / "koro.txt");
    const std::filesystem::path rsc_path = rsc / "tiki.txt";
    text_sptr tiki_sptr = rmanager.load<text>(rsc_path);
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    text_sptr koro_2_sptr = rmanager.load<text>(rsc / "koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_NE(koro_sptr, koro_2_sptr);
    ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
}

TEST(resource_manager_tests, remove__rsc_path__no_error)
{
    std::filesystem::path rsc = textdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.load<text>(rsc / "koro.txt");
    text_sptr tiki_sptr = rmanager.load<text>(rsc / "tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    rmanager.remove<text>(rsc / "koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
    const std::filesystem::path rsc_path = rsc / "tiki.txt";
    rmanager.remove<text>(rsc_path);
    ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
}

TEST(resource_manager_tests, get_shared__vlfs_resource_file_exists__no_exception)
{
    std::filesystem::path rsc = textdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.get_shared<text>("TEXT:/koro.txt");
    const std::filesystem::path rsc_path = "TEXT:/koro.txt";
    text_sptr koro_sptr_2 = rmanager.get_shared<text>(rsc_path);
    ASSERT_NE(koro_sptr, nullptr);
    ASSERT_EQ(koro_sptr, koro_sptr_2);
    ASSERT_EQ(koro_sptr->contents, koro_contents());

    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
}

TEST(resource_manager_tests, get__vlfs_resource_file_exists__no_exception)
{
    std::filesystem::path rsc = textdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text& koro_ref = rmanager.get<text>("TEXT:/koro.txt");
    text& koro_ref2 = rmanager.get<text>("TEXT:/koro.txt");
    rmanager.get<text>("TEXT:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_EQ(&koro_ref, &koro_ref2);
}

TEST(resource_manager_tests, insert__vlfs_rsc_sptr__no_error)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    bool ins_res = rmanager.insert("default_tale", tale_sptr);
    ASSERT_TRUE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text>("default_tale"));
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    ins_res = rmanager.insert("default_tale", tale_2_sptr);
    ASSERT_FALSE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text>("default_tale"));
}

TEST(resource_manager_tests, set__vlfs_rsc_sptr__no_error)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
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

TEST(resource_manager_tests, load__vlfs_resource_file_exists__expect_no_exception)
{
    std::filesystem::path rsc = textdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.load<text>("TEXT:/koro.txt");
    const std::filesystem::path rsc_path = "TEXT:/tiki.txt";
    text_sptr tiki_sptr = rmanager.load<text>(rsc_path);
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    text_sptr koro_2_sptr = rmanager.load<text>("TEXT:/koro.txt", std::nothrow);
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    text_sptr tiki_2_sptr = rmanager.load<text>(rsc_path, std::nothrow);
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_NE(koro_sptr, koro_2_sptr);
    ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
}

TEST(resource_manager_tests, remove__vlfs_rsc_path__no_error)
{
    std::filesystem::path rsc = textdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.load<text>("TEXT:/koro.txt");
    text_sptr tiki_sptr = rmanager.load<text>("TEXT:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    rmanager.remove<text>("TEXT:/koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
    const std::filesystem::path rsc_path = "TEXT:/tiki.txt";
    rmanager.remove<text>(rsc_path);
    ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
}
