#include <rsce/resource_store.hpp>
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

    bool load_from_file(const std::filesystem::path& fpath)
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

static_assert(rsce::traits::is_loadable_resource_v<text>);

using text_sptr = rsce::resource_store<text>::resource_sptr;

namespace rsce
{
template class default_resource_store<text>;
}

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

TEST(resource_store_tests, test_constructor)
{
    rsce::resource_store<text> text_store;
    ASSERT_EQ(text_store.size(), 0);
}

TEST(resource_store_tests, test_get)
{
    std::filesystem::path rsc = rscdir();

    rsce::resource_store<text> text_store;
    text_sptr koro_sptr = text_store.get(rsc/"koro.txt");
    text_sptr koro_sptr_2 = text_store.get(rsc/"koro.txt");
    ASSERT_NE(koro_sptr, nullptr);
    ASSERT_EQ(koro_sptr, koro_sptr_2);
    ASSERT_EQ(koro_sptr->contents, koro_contents());

    ASSERT_EQ(text_store.size(), 1);
}

TEST(resource_store_tests, test_clear)
{
    std::filesystem::path rsc = rscdir();

    rsce::resource_store<text> text_store;
    text_store.get(rsc/"koro.txt");
    text_store.get(rsc/"tiki.txt");
    ASSERT_EQ(text_store.size(), 2);
    text_store.clear();
    ASSERT_EQ(text_store.size(), 0);
}

TEST(resource_store_tests, test_insert)
{
    rsce::resource_store<text> text_store;
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    bool ins_res = text_store.insert("default_tale", tale_sptr);
    ASSERT_TRUE(ins_res);
    ASSERT_EQ(tale_sptr, text_store.get("default_tale"));
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    ins_res = text_store.insert("default_tale", tale_2_sptr);
    ASSERT_FALSE(ins_res);
    ASSERT_EQ(tale_sptr, text_store.get("default_tale"));
}

TEST(resource_store_tests, test_set)
{
    rsce::resource_store<text> text_store;
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    text* tale_ptr = tale_sptr.get();
    text_store.set("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, text_store.get("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    text* tale_2_ptr = tale_2_sptr.get();
    text_store.set("default_tale", std::move(tale_2_sptr));
    ASSERT_EQ(tale_2_ptr, text_store.get("default_tale").get());
    ASSERT_EQ(tale_2_ptr->contents, "Once upon a time 2");
}

TEST(resource_store_tests, test_set_2)
{
    rsce::resource_store<text> rmanager;
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    text* tale_ptr = tale_sptr.get();
    rmanager.set("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text tale_2{"Once upon a time 2"};
    rmanager.set("default_tale", std::move(tale_2));
    ASSERT_EQ(tale_ptr, rmanager.get("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time 2");
}

TEST(resource_store_tests, test_load)
{
    std::filesystem::path rsc = rscdir();

    rsce::resource_store<text> text_store;
    text_sptr koro_sptr = text_store.load(rsc/"koro.txt");
    text_sptr tiki_sptr = text_store.load(rsc/"tiki.txt");
    ASSERT_EQ(text_store.size(), 2);
    text_sptr koro_2_sptr = text_store.load(rsc/"koro.txt");
    ASSERT_EQ(text_store.size(), 2);
    ASSERT_NE(koro_sptr, koro_2_sptr);
    ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
}

TEST(resource_store_tests, test_remove)
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
