#include <iostream>
#include <rsce/basic_resource_manager.hpp>
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
    stream << "Once upon a time, a mighty spirit...";
    stream.close();

    rsce::basic_resource_manager rmanager;
    const text& tale = rmanager.get<text>(std::filesystem::temp_directory_path()/"rsce/rsc/tale.txt");

    std::cout << tale.contents << std::endl;

    return EXIT_SUCCESS;
}
