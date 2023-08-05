#include <arba/rsce/basic_resource_manager.hpp>
#include <gtest/gtest.h>
#include <string>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <array>

class text
{
public:
    std::string contents;

    inline bool operator<=>(const text&) const = default;

    bool load_from_file(const std::filesystem::path& fpath, rsce::basic_resource_manager& rmanager)
    {
        std::ifstream stream(fpath);
        stream.seekg(0, std::ios::end);
        contents.reserve(stream.tellg());
        stream.seekg(0, std::ios::beg);
        contents.assign((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        return true;
    }
};

namespace std
{
template<> struct hash<text>
{
    std::size_t operator()(text const& value) const noexcept
    {
        return std::hash<std::string>{}(value.contents);
    }
};
}

using text_sptr = rsce::resource_store<text>::resource_sptr;

// Resources:

const std::string& koro_contents()
{
    static std::string contents = "koro koro\nkoro";
    return contents;
}

const std::string& tiki_contents()
{
    static std::string contents = "Tiki Tiki tiki.";
    return contents;
}

std::filesystem::path rscdir()
{
    static std::filesystem::path dirpath = []()
    {
        auto path = std::filesystem::temp_directory_path()/"rsce/rsc";
        std::filesystem::create_directories(path);
        return path;
    }();
    return dirpath;
}

void create_resource_files()
{
    std::filesystem::path rsc = rscdir();

    std::ofstream stream(rsc/"koro.txt");
    stream << koro_contents();
    stream.close();

    stream.open(rsc/"tiki.txt");
    stream << tiki_contents();
    stream.close();
}

// Unit tests:

TEST(basic_resource_manager_tests_2, test_constructor)
{
    rsce::basic_resource_manager rmanager;
}

TEST(basic_resource_manager_tests_2, test_get)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    text_sptr koro_sptr = rmanager.get_shared<text>(rsc/"koro.txt");
    text_sptr koro_sptr_2 = rmanager.get_shared<text>(rsc/"koro.txt");
    ASSERT_NE(koro_sptr, nullptr);
    ASSERT_EQ(koro_sptr, koro_sptr_2);
    ASSERT_EQ(koro_sptr->contents, koro_contents());

    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
}

TEST(basic_resource_manager_tests_2, test_get_ref)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    text& koro_ref = rmanager.get<text>(rsc/"koro.txt");
    text& koro_ref2 = rmanager.get<text>(rsc/"koro.txt");
    rmanager.get<text>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_EQ(&koro_ref, &koro_ref2);
}

TEST(basic_resource_manager_tests_2, test_insert)
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

TEST(basic_resource_manager_tests_2, test_set)
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

TEST(basic_resource_manager_tests_2, test_set_2)
{
    rsce::basic_resource_manager rmanager;
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    text* tale_ptr = tale_sptr.get();
    rmanager.set<text>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get_shared<text>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text tale_2{"Once upon a time 2"};
    rmanager.set<text>("default_tale", std::move(tale_2));
    ASSERT_EQ(tale_ptr, rmanager.get_shared<text>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time 2");
}

TEST(basic_resource_manager_tests_2, test_load)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    text_sptr koro_sptr = rmanager.load<text>(rsc/"koro.txt");
    text_sptr tiki_sptr = rmanager.load<text>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    text_sptr koro_2_sptr = rmanager.load<text>(rsc/"koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_NE(koro_sptr, koro_2_sptr);
    ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
}

TEST(basic_resource_manager_tests_2, test_remove)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    text_sptr koro_sptr = rmanager.load<text>(rsc/"koro.txt");
    text_sptr tiki_sptr = rmanager.load<text>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    rmanager.remove<text>(rsc/"koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
    rmanager.remove<text>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
}

TEST(basic_resource_manager_tests_2, test_store)
{
    std::filesystem::path rsc = rscdir();

    rsce::basic_resource_manager rmanager;
    text_sptr tiki_sptr = rmanager.load<text>(rsc/"tiki.txt");
    rsce::resource_store<text>& text_store = rmanager.store<text>();
    text_sptr tiki_sptr_2 = text_store.get_shared(rsc/"tiki.txt", rmanager);
    ASSERT_EQ(text_store.size(), 1);
    ASSERT_EQ(tiki_sptr, tiki_sptr_2);
}

class red_text : public text
{
};

class green_text : public text
{
};

TEST(basic_resource_manager_tests_2, test_creation_order)
{
    std::filesystem::path rsc = rscdir();

    {
        rsce::basic_resource_manager rmanager;
        ASSERT_NE(rmanager.get_shared<red_text>(rsc/"tiki.txt"), nullptr);
        ASSERT_NE(rmanager.get_shared<green_text>(rsc/"tiki.txt"), nullptr);
    }
    {
        rsce::basic_resource_manager rmanager;
        text_sptr green_tiki_sptr = rmanager.get_shared<green_text>(rsc/"tiki.txt");
        ASSERT_NE(rmanager.load<red_text>(rsc/"tiki.txt"), nullptr);
        text_sptr green_tiki_sptr_2 = rmanager.get_shared<green_text>(rsc/"tiki.txt");
        ASSERT_EQ(green_tiki_sptr, green_tiki_sptr_2);
    }
}

int main(int argc, char** argv)
{
    create_resource_files();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
