#include "resources/resources_helper.hpp"
#include "resources/text_mngr.hpp"
#include <arba/rsce/basic_resource_manager.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <string>

using text_mngr_sptr = rsce::resource_store<text_mngr>::resource_sptr;

// Unit tests:

TEST(basic_resource_manager_mngr_tests, get_shared__resource_file_exists__no_exception)
{
    std::filesystem::path rsc = textdir();

    rsce::basic_resource_manager rmanager;
    text_mngr_sptr koro_sptr = rmanager.get_shared<text_mngr>(rsc / "koro.txt");
    text_mngr_sptr koro_sptr_2 = rmanager.get_shared<text_mngr>(rsc / "koro.txt");
    ASSERT_NE(koro_sptr, nullptr);
    ASSERT_EQ(koro_sptr, koro_sptr_2);
    ASSERT_EQ(koro_sptr->contents, koro_contents());

    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 1);
}

TEST(basic_resource_manager_mngr_tests, load__resource_file_exists__expect_no_exception)
{
    std::filesystem::path rsc = textdir();

    rsce::basic_resource_manager rmanager;
    text_mngr_sptr koro_sptr = rmanager.load<text_mngr>(rsc / "koro.txt");
    text_mngr_sptr tiki_sptr = rmanager.load<text_mngr>(rsc / "tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    text_mngr_sptr koro_2_sptr = rmanager.load<text_mngr>(rsc / "koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text_mngr>(), 2);
    ASSERT_NE(koro_sptr, koro_2_sptr);
    ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
}
