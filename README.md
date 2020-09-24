# Concept

The purpose is to provide resource managing tools in C++.

- `resource_store<RSC>` which stores instances of `RSC`.
- `basic_resource_manager` which embeds *resource stores* of different types.
- `resource_manager` which embeds *resource stores* of different types, and uses a *virtual filesystem* so that resources can be gotten with a real or virtual filesystem path. (cf. [vlfs](https://github.com/arapelle/vlfs) for more details)

See [task board](https://app.gitkraken.com/glo/board/X2saif5v8AAR4XXb) for future updates and features.

# Install

## Requirements

Binaries:

- A C++20 compiler (ex: g++-10)
- CMake 3.16 or later

Libraries:

- [strn](https://github.com/arapelle/strn) 0.1.4
- [vlfs](https://github.com/arapelle/vlfs) 0.1.0
- [Google Test](https://github.com/google/googletest) 1.10 or later (only for testing)

## Clone

```
git clone https://github.com/arapelle/rsce --recurse-submodules
```

## Quick Install

There is a cmake script at the root of the project which builds the library in *Release* mode and install it (default options are used).

```
cd /path/to/rsce
cmake -P cmake_quick_install.cmake
```

Use the following to quickly install a different mode.

```
cmake -DCMAKE_BUILD_TYPE=Debug -P cmake_quick_install.cmake
```

## Uninstall

There is a uninstall cmake script created during installation. You can use it to uninstall properly this library.

```
cd /path/to/installed-rsce/
cmake -P cmake_uninstall.cmake
```

# How to use

## Example - Load a text resource with a *resource_manager*

```c++
#include <iostream>
#include <rsce/resource_manager.hpp>
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

## Example - Using *rsce* in a CMake project

See the [basic cmake project](https://github.com/arapelle/rsce/tree/master/example/basic_cmake_project) example, and more specifically the [CMakeLists.txt](https://github.com/arapelle/rsce/tree/master/example/basic_cmake_project/CMakeLists.txt) to see how to use *rsce* in your CMake projects.

# License

[MIT License](https://github.com/arapelle/rsce/blob/master/LICENSE.md) © rsce