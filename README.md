# Concept

The purpose is to provide resource managing tools in C++.

- `resource_store<RSC>` which stores instances of `RSC`.
- `basic_resource_manager` which embeds *resource stores* of different types.
- `resource_manager` which embeds *resource stores* of different types, and uses a *virtual filesystem* so that resources can be gotten with a real or virtual filesystem path. (cf. [vlfs](https://github.com/arapelle/vlfs) for more details)

# Install

## Requirements

Binaries:
- A C++20 compiler (ex: g++-10)
- CMake 3.26 or later

Libraries:
- [arba-core](https://github.com/arapelle/arba-core) 0.20.0
- [arba-vlfs](https://github.com/arapelle/arba-vlfs) 0.3.0

Testing Libraries (optional):
- [Google Test](https://github.com/google/googletest) 1.13 or later  (optional)

## Clone

```
git clone https://github.com/arapelle/arba-rsce --recurse-submodules
```

## Quick Install

There is a cmake script at the root of the project which builds the library in *Release* mode and install it (default options are used).

```
cd /path/to/arba-rsce
cmake -P cmake/scripts/quick_install.cmake
```

Use the following to quickly install a different mode.

```
cmake -P cmake/scripts/quick_install.cmake -- TESTS BUILD Debug DIR /tmp/local
```

## Uninstall

There is a uninstall cmake script created during installation. You can use it to uninstall properly this library.

```
cd /path/to/installed-arba-rsce/
cmake -P uninstall.cmake
```

# How to use

## Example - Load a text resource with a *resource_manager*

```c++
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

```

## Example - Using *arba-rsce* in a CMake project

See the *basic_cmake_project* example, and more specifically the *CMakeLists.txt* to see how to use *arba-rsce* in your CMake projects.

# License

[MIT License](./LICENSE.md) © arba-rsce
