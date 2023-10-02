#pragma once

#include <arba/vlfs/vlfs.hpp>

inline const std::string& koro_contents()
{
    static const std::string contents = "koro koro\nkoro";
    return contents;
}

inline const std::string& tiki_contents()
{
    static const std::string contents = "Tiki Tiki tiki.";
    return contents;
}

inline const std::string& invalid_contents()
{
    static const std::string contents = "0123456789 invalid";
    return contents;
}

inline std::filesystem::path rscdir()
{
    static const std::filesystem::path dirpath = []
    {
        auto path = std::filesystem::temp_directory_path()/"rsce/rsc";
        std::filesystem::create_directories(path);
        return path;
    }();
    return dirpath;
}

inline void create_resource_files()
{
    const std::filesystem::path rsc = rscdir();

    std::ofstream stream(rsc/"koro.txt");
    stream << koro_contents();
    stream.close();

    stream.open(rsc/"tiki.txt");
    stream << tiki_contents();
    stream.close();

    stream.open(rsc/"invalid.txt");
    stream << invalid_contents();
    stream.close();
}

inline vlfs::virtual_filesystem create_vlfs()
{
    vlfs::virtual_filesystem vlfs;
    vlfs.set_virtual_root(strn::string64("TMP"), std::filesystem::temp_directory_path());
    vlfs.set_virtual_root(strn::string64("RSCE"), "TMP:/rsce");
    vlfs.set_virtual_root(strn::string64("RSC"), "RSCE:/rsc");
    return vlfs;
}
