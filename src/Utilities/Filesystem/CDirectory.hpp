#pragma once
#include <Utilities/Filesystem/Directory.hpp>
#include <Utilities/Allocator.hpp>
#include <filesystem>

namespace CrossFire::detail
{

class CDirectory final : public DirectoryBase {
    std::filesystem::path fs_path;

public:
    CDirectory(const char *path);
    ~CDirectory() override;

    auto open_file(const char *name, const char *mode)
        -> UniquePtr<FileBase> override;
    auto open_directory(const char *name) -> DirectoryBase override;

    auto create_file(const char *name, const char *mode)
        -> UniquePtr<FileBase> override;
    auto delete_file(const char *name) -> void override;
    auto exists(const char *name) -> bool override;
    auto is_directory(const char *name) -> bool override;

    auto create_directory(const char *name) -> void override;
    auto delete_directory(const char *name) -> void override;
};

}