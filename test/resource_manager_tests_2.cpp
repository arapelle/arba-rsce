#include <rsce/resource_manager.hpp>
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

    bool load_from_file(const std::filesystem::path& fpath, rsce::resource_manager& rmanager)
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

vlfs::virtual_filesystem create_vlfs()
{
    vlfs::virtual_filesystem vlfs;
    vlfs.set_virtual_root(strn::string64("TMP"), std::filesystem::temp_directory_path());
    vlfs.set_virtual_root(strn::string64("RSCE"), "TMP:/rsce");
    vlfs.set_virtual_root(strn::string64("RSC"), "RSCE:/rsc");
    return vlfs;
}

// Unit tests:

TEST(resource_manager_tests_2, test_constructor)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    ASSERT_EQ(rmanager.virtual_filesystem(), &vlfs);
}

TEST(resource_manager_tests_2, test_get)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.get<text>(rsc/"koro.txt");
    text_sptr koro_sptr_2 = rmanager.get<text>(rsc/"koro.txt");
    ASSERT_NE(koro_sptr, nullptr);
    ASSERT_EQ(koro_sptr, koro_sptr_2);
    ASSERT_EQ(koro_sptr->contents, koro_contents());

    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
}

TEST(resource_manager_tests_2, test_get_ref)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text& koro_ref = rmanager.get_ref<text>(rsc/"koro.txt");
    text& koro_ref2 = rmanager.get_ref<text>(rsc/"koro.txt");
    rmanager.get_ref<text>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_EQ(&koro_ref, &koro_ref2);
}

TEST(resource_manager_tests_2, test_insert)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    bool ins_res = rmanager.insert("default_tale", tale_sptr);
    ASSERT_TRUE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get<text>("default_tale"));
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    ins_res = rmanager.insert("default_tale", tale_2_sptr);
    ASSERT_FALSE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get<text>("default_tale"));
}

TEST(resource_manager_tests_2, test_set)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    text* tale_ptr = tale_sptr.get();
    rmanager.set<text>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get<text>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    text* tale_2_ptr = tale_2_sptr.get();
    rmanager.set<text>("default_tale", std::move(tale_2_sptr));
    ASSERT_EQ(tale_2_ptr, rmanager.get<text>("default_tale").get());
    ASSERT_EQ(tale_2_ptr->contents, "Once upon a time 2");
}

TEST(resource_manager_tests_2, test_set_2)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    text* tale_ptr = tale_sptr.get();
    rmanager.set<text>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get<text>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text tale_2{"Once upon a time 2"};
    rmanager.set<text>("default_tale", std::move(tale_2));
    ASSERT_EQ(tale_ptr, rmanager.get<text>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time 2");
}

TEST(resource_manager_tests_2, test_load)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.load<text>(rsc/"koro.txt");
    text_sptr tiki_sptr = rmanager.load<text>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    text_sptr koro_2_sptr = rmanager.load<text>(rsc/"koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_NE(koro_sptr, koro_2_sptr);
    ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
}

TEST(resource_manager_tests_2, test_remove)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.load<text>(rsc/"koro.txt");
    text_sptr tiki_sptr = rmanager.load<text>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    rmanager.remove<text>(rsc/"koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
    rmanager.remove<text>(rsc/"tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
}

TEST(resource_manager_tests_2, test_get_vlfs)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.get<text>("RSC:/koro.txt");
    text_sptr koro_sptr_2 = rmanager.get<text>("RSC:/koro.txt");
    ASSERT_NE(koro_sptr, nullptr);
    ASSERT_EQ(koro_sptr, koro_sptr_2);
    ASSERT_EQ(koro_sptr->contents, koro_contents());

    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
}

TEST(resource_manager_tests_2, test_get_ref_vlfs)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text& koro_ref = rmanager.get_ref<text>("RSC:/koro.txt");
    text& koro_ref2 = rmanager.get_ref<text>("RSC:/koro.txt");
    rmanager.get_ref<text>("RSC:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_EQ(&koro_ref, &koro_ref2);
}

TEST(resource_manager_tests_2, test_insert_vlfs)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    bool ins_res = rmanager.insert("default_tale", tale_sptr);
    ASSERT_TRUE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get<text>("default_tale"));
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    ins_res = rmanager.insert("default_tale", tale_2_sptr);
    ASSERT_FALSE(ins_res);
    ASSERT_EQ(tale_sptr, rmanager.get<text>("default_tale"));
}

TEST(resource_manager_tests_2, test_set_vlfs)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    text* tale_ptr = tale_sptr.get();
    rmanager.set<text>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get<text>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text_sptr tale_2_sptr = std::make_shared<text>("Once upon a time 2");
    text* tale_2_ptr = tale_2_sptr.get();
    rmanager.set<text>("default_tale", std::move(tale_2_sptr));
    ASSERT_EQ(tale_2_ptr, rmanager.get<text>("default_tale").get());
    ASSERT_EQ(tale_2_ptr->contents, "Once upon a time 2");
}

TEST(resource_manager_tests_2, test_set_2_vlfs)
{
    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr tale_sptr = std::make_shared<text>("Once upon a time");
    text* tale_ptr = tale_sptr.get();
    rmanager.set<text>("default_tale", std::move(tale_sptr));
    ASSERT_EQ(tale_ptr, rmanager.get<text>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time");
    text tale_2{"Once upon a time 2"};
    rmanager.set<text>("default_tale", std::move(tale_2));
    ASSERT_EQ(tale_ptr, rmanager.get<text>("default_tale").get());
    ASSERT_EQ(tale_ptr->contents, "Once upon a time 2");
}

TEST(resource_manager_tests_2, test_load_vlfs)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.load<text>("RSC:/koro.txt");
    text_sptr tiki_sptr = rmanager.load<text>("RSC:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    text_sptr koro_2_sptr = rmanager.load<text>("RSC:/koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    ASSERT_NE(koro_sptr, koro_2_sptr);
    ASSERT_EQ(koro_sptr->contents, koro_2_sptr->contents);
}

TEST(resource_manager_tests_2, test_remove_vlfs)
{
    std::filesystem::path rsc = rscdir();

    vlfs::virtual_filesystem vlfs = create_vlfs();
    rsce::resource_manager rmanager(vlfs);
    text_sptr koro_sptr = rmanager.load<text>("RSC:/koro.txt");
    text_sptr tiki_sptr = rmanager.load<text>("RSC:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 2);
    rmanager.remove<text>("RSC:/koro.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 1);
    rmanager.remove<text>("RSC:/tiki.txt");
    ASSERT_EQ(rmanager.number_of_resources<text>(), 0);
}

int main(int argc, char** argv)
{
    create_resource_files();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
