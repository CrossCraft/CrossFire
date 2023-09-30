#pragma once
#include "../Types.hpp"
#include "File.hpp"
#include "../Allocator.hpp"

namespace CrossFire
{

class DirectoryBase {
    const char *path;

public:
    explicit DirectoryBase(const char *path)
        : path(path)
    {
        (void)this->path;
    };
    virtual ~DirectoryBase() = default;

    inline virtual auto open_file(const char *name, const char *mode)
        -> UniquePtr<FileBase>
    {
        (void)name;
        (void)mode;
        cf_assert(false, "Unimplemented base function called.");
        return UniquePtr<FileBase>(nullptr, stack_allocator);
    };

    inline virtual auto open_directory(const char *name) -> DirectoryBase
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
        return DirectoryBase("");
    }

    virtual auto create_file(const char *name, const char *mode)
        -> UniquePtr<FileBase>
    {
        (void)name;
        (void)mode;
        cf_assert(false, "Unimplemented base function called.");
        return UniquePtr<FileBase>(nullptr, stack_allocator);
    }

    virtual auto delete_file(const char *name) -> void
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
    }

    virtual auto exists(const char *name) -> bool
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
        return false;
    }

    virtual auto is_directory(const char *name) -> bool
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
        return false;
    }

    virtual auto create_directory(const char *name) -> void
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
    }

    virtual auto delete_directory(const char *name) -> void
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
    }
};

class DirectoryFactory {
public:
    static auto cwd() -> DirectoryBase &;
    static auto open(const char *path) -> DirectoryBase;
};

}