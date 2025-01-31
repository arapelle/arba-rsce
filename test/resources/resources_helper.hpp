#pragma once

#include <arba/vlfs/vlfs.hpp>

#include <fstream>

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

inline const std::filesystem::path& rscdir()
{
    static const std::filesystem::path root_dpath = std::filesystem::path(RSC_PATH);
    return root_dpath;
}

inline const std::filesystem::path& textdir()
{
    static const std::filesystem::path root_dpath = rscdir() / "ut" / "text";
    return root_dpath;
}

inline vlfs::virtual_filesystem create_vlfs()
{
    vlfs::virtual_filesystem vlfs;
    vlfs.set_virtual_root(strn::string64("RSC"), rscdir());
    vlfs.set_virtual_root(strn::string64("UT"), "RSC:/ut");
    vlfs.set_virtual_root(strn::string64("TEXT"), "UT:/text");
    return vlfs;
}
