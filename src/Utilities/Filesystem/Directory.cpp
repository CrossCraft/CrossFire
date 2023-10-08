#include "CDirectory.hpp"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace CrossFire::detail
{

CDirectory::CDirectory(const char *path)
    : DirectoryBase(path)
    , fs_path(path)
{
    PROFILE_ZONE;
    cf_assert(exists(path), "Directory does not exist.");
    cf_assert(is_directory(path), "Path is not a directory.");
}
CDirectory::~CDirectory()
{
    PROFILE_ZONE;
    // Nothing to do here.
}

auto CDirectory::open_file(const char *name, const char *mode)
    -> UniquePtr<FileBase>
{
    PROFILE_ZONE;
    auto path = fs_path / std::filesystem::path(name);
    cf_assert(fs::exists(path), "File does not exist.");
    cf_assert(!fs::is_directory(path), "Path is a directory.");

    return FileFactory::open(path.string().c_str(), mode);
}
auto CDirectory::open_directory(const char *name) -> DirectoryBase
{
    PROFILE_ZONE;
    auto path = fs_path / std::filesystem::path(name);
    cf_assert(fs::exists(path), "Directory does not exist.");
    cf_assert(fs::is_directory(path), "Path is not a directory.");

    return DirectoryFactory::open(path.string().c_str());
}

auto CDirectory::create_file(const char *name, const char *mode)
    -> UniquePtr<FileBase>
{
    PROFILE_ZONE;
    // Create relative to this directory.
    return FileFactory::open(
        (fs_path / std::filesystem::path(name)).string().c_str(), mode);
}
auto CDirectory::delete_file(const char *name) -> void
{
    PROFILE_ZONE;
    // Delete relative to this directory.
    fs::remove(fs_path / std::filesystem::path(name));
}

auto CDirectory::exists(const char *name) -> bool
{
    PROFILE_ZONE;
    // Check if the path exists.
    return fs::exists(name);
}
auto CDirectory::is_directory(const char *name) -> bool
{
    PROFILE_ZONE;
    // Check if the path is a directory.
    return fs::is_directory(name);
}

auto CDirectory::create_directory(const char *name) -> void
{
    PROFILE_ZONE;
    // Create relative to this directory.
    fs::create_directory(fs_path / std::filesystem::path(name));
}

auto CDirectory::delete_directory(const char *name) -> void
{
    PROFILE_ZONE;
    // Delete relative to this directory.
    fs::remove(fs_path / std::filesystem::path(name));
}

}

namespace CrossFire
{

auto DirectoryFactory::open(const char *path) -> DirectoryBase
{
    PROFILE_ZONE;
    return detail::CDirectory(path);
}

auto DirectoryFactory::cwd() -> DirectoryBase &
{
    PROFILE_ZONE;
    static detail::CDirectory cwd(".");
    return cwd;
}

}