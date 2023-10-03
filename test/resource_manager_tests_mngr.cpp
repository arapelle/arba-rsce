#include <arba/rsce/resource_manager.hpp>
#include <gtest/gtest.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <array>
#include "resources/text_mngr.hpp"
#include "resources/resources_helper.hpp"

using text_mngr_sptr = rsce::resource_store<text_mngr>::resource_sptr;

// Unit tests:

TEST(resource_manager_tests_2, test_constructor)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    ASSERT_EQ(&rmanager.virtual_filesystem(), &vlfs);
}

TEST(resource_manager_tests_2, test_get)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr koro_sptr = rmanager.get_shared<text_mngr>(rsc/"koro.txt");
    text_mngr_sptr koro_sptr_2 = rmanager.get_shared<text_mngr>(rsc/"koro.txt");
    ASSERT_NE(koro_sptr, nullptr);
    ASSERT_EQ(koro_sptr, koro_sptr_2);
    ASSERT_EQ(koro_sptr->contents, koro_contents());

    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 1);
}

TEST(resource_manager_tests_2, test_get_ref)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr& koro_ref = rmanager.get<text_mngr>(rsc/"koro.txt");
    text_mngr& koro_ref2 = rmanager.get<text_mngr>(rsc/"koro.txt");
    rmanager.get<text_mngr>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    ASSERT_EQ(&koro_ref, &koro_ref2);
}

TEST(resource_manager_tests_2, test_insert)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr tale_sptr = std::make_shared<text_mngr>("Once upon a time");
    bool ins_res = rmanager.insert("default_tale", tale_sptr);
    ASSERT_TRUE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text_mngr>("default_tale"));
    text_mngr_sptr tale_2_sptr = std::make_shared<text_mngr>("Once upon a time 2");
    ins_res = rmanager.insert("default_tale", tale_2_sptr);
    ASSERT_FALSE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text_mngr>("default_tale"));
}

TEST(resource_manager_tests_2, test_set)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr tale_sptr = std::make_shared<text_mngr>("Once upon a time");
    text_mngr* tale_ptr = tale_sptr.get();
    rmanager.set<text_mngr>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get_shared<text_mngr>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text_mngr_sptr tale_2_sptr = std::make_shared<text_mngr>("Once upon a time 2");
    text_mngr* tale_2_ptr = tale_2_sptr.get();
    rmanager.set<text_mngr>("default_tale", std::move(tale_2_sptr));
    ASSERT_EQ(tale_2_ptr, rmanager.get_shared<text_mngr>("default_tale").get());
    ASSERT_EQ(tale_2_ptr->contents, "Once upon a time 2");
}

TEST(resource_manager_tests_2, test_set_2)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr tale_sptr = std::make_shared<text_mngr>("Once upon a time");
    text_mngr* tale_ptr = tale_sptr.get();
    rmanager.set<text_mngr>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get_shared<text_mngr>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text_mngr tale_2{"Once upon a time 2"};
    rmanager.set<text_mngr>("default_tale", std::move(tale_2));
    ASSERT_EQ(tale_ptr, rmanager.get_shared<text_mngr>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time 2");
}

TEST(resource_manager_tests_2, test_load)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr koro_sptr = rmanager.load<text_mngr>(rsc/"koro.txt");
    text_mngr_sptr tiki_sptr = rmanager.load<text_mngr>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    text_mngr_sptr koro_2_sptr = rmanager.load<text_mngr>(rsc/"koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    ASSERT_NE(koro_sptr, koro_2_sptr);
    ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
}

TEST(resource_manager_tests_2, test_remove)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr koro_sptr = rmanager.load<text_mngr>(rsc/"koro.txt");
    text_mngr_sptr tiki_sptr = rmanager.load<text_mngr>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    rmanager.remove<text_mngr>(rsc/"koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 1);
    rmanager.remove<text_mngr>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 0);
}

TEST(resource_manager_tests_2, test_get_vlfs)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr koro_sptr = rmanager.get_shared<text_mngr>("RSC:/koro.txt");
    text_mngr_sptr koro_sptr_2 = rmanager.get_shared<text_mngr>("RSC:/koro.txt");
    ASSERT_NE(koro_sptr, nullptr);
    ASSERT_EQ(koro_sptr, koro_sptr_2);
    ASSERT_EQ(koro_sptr->contents, koro_contents());

    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 1);
}

TEST(resource_manager_tests_2, test_get_ref_vlfs)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr& koro_ref = rmanager.get<text_mngr>("RSC:/koro.txt");
    text_mngr& koro_ref2 = rmanager.get<text_mngr>("RSC:/koro.txt");
    rmanager.get<text_mngr>("RSC:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    ASSERT_EQ(&koro_ref, &koro_ref2);
}

TEST(resource_manager_tests_2, test_insert_vlfs)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr tale_sptr = std::make_shared<text_mngr>("Once upon a time");
    bool ins_res = rmanager.insert("default_tale", tale_sptr);
    ASSERT_TRUE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text_mngr>("default_tale"));
    text_mngr_sptr tale_2_sptr = std::make_shared<text_mngr>("Once upon a time 2");
    ins_res = rmanager.insert("default_tale", tale_2_sptr);
    ASSERT_FALSE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get_shared<text_mngr>("default_tale"));
}

TEST(resource_manager_tests_2, test_set_vlfs)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr tale_sptr = std::make_shared<text_mngr>("Once upon a time");
    text_mngr* tale_ptr = tale_sptr.get();
    rmanager.set<text_mngr>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get_shared<text_mngr>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text_mngr_sptr tale_2_sptr = std::make_shared<text_mngr>("Once upon a time 2");
    text_mngr* tale_2_ptr = tale_2_sptr.get();
    rmanager.set<text_mngr>("default_tale", std::move(tale_2_sptr));
    ASSERT_EQ(tale_2_ptr, rmanager.get_shared<text_mngr>("default_tale").get());
    ASSERT_EQ(tale_2_ptr->contents, "Once upon a time 2");
}

TEST(resource_manager_tests_2, test_set_2_vlfs)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr tale_sptr = std::make_shared<text_mngr>("Once upon a time");
    text_mngr* tale_ptr = tale_sptr.get();
    rmanager.set<text_mngr>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get_shared<text_mngr>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text_mngr tale_2{"Once upon a time 2"};
    rmanager.set<text_mngr>("default_tale", std::move(tale_2));
    ASSERT_EQ(tale_ptr, rmanager.get_shared<text_mngr>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time 2");
}

TEST(resource_manager_tests_2, test_load_vlfs)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr koro_sptr = rmanager.load<text_mngr>("RSC:/koro.txt");
    text_mngr_sptr tiki_sptr = rmanager.load<text_mngr>("RSC:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    text_mngr_sptr koro_2_sptr = rmanager.load<text_mngr>("RSC:/koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    ASSERT_NE(koro_sptr, koro_2_sptr);
    ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
}

TEST(resource_manager_tests_2, test_remove_vlfs)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_mngr_sptr koro_sptr = rmanager.load<text_mngr>("RSC:/koro.txt");
    text_mngr_sptr tiki_sptr = rmanager.load<text_mngr>("RSC:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    rmanager.remove<text_mngr>("RSC:/koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 1);
    rmanager.remove<text_mngr>("RSC:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 0);
}

int main(int argc, char** argv)
{
    create_resource_files();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
