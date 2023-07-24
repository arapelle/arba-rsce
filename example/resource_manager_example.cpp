#include <iostream>
#include <arba/rsce/resource_manager.hpp>
#include <fstream>

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

int main()
{
    std::filesystem::create_directories(std::filesystem::temp_directory_path()/"rsce/rsc");
    std::ofstream stream(std::filesystem::temp_directory_path()/"rsce/rsc/tale.txt");
    stream << "Once upon a time...";
    stream.close();

    vlfs::virtual_filesystem vlfs;
    vlfs.set_virtual_root("TMP", std::filesystem::temp_directory_path());
    vlfs.set_virtual_root("RSCE", "TMP:/rsce");
    vlfs.set_virtual_root("RSC", "RSCE:/rsc");

    rsce::resource_manager rmanager(vlfs);
    const text& tale = rmanager.get<text>("RSC:/tale.txt");
    const text& tale_2 = rmanager.get<text>(std::filesystem::temp_directory_path()/"rsce/rsc/tale.txt");
    std::cout << tale.contents << std::endl;
    std::cout << tale_2.contents << std::endl;

    return EXIT_SUCCESS;
}
