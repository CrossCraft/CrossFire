#pragma once
#include "../Types.hpp"
#include "File.hpp"
#include "../Allocator.hpp"

namespace CrossFire
{

/**
 * @brief DirectoryBase is an interface for directory objects.
 */
class DirectoryBase {
    const char *path;

public:
    /**
     * @brief Construct a new Directory object.
     * @param path The path to the directory.
     */
    explicit DirectoryBase(const char *path)
        : path(path)
    {
        (void)this->path;
    };
    virtual ~DirectoryBase() = default;

    /**
     * @brief Open a file.
     * @param name The name of the file.
     * @param mode The mode to open the file in.
     * @return The file.
     */
    inline virtual auto open_file(const char *name, const char *mode)
        -> UniquePtr<FileBase>
    {
        (void)name;
        (void)mode;
        cf_assert(false, "Unimplemented base function called.");
        return UniquePtr<FileBase>(nullptr, stack_allocator);
    };

    /**
     * @brief Open a directory.
     * @param name The name of the directory.
     * @return The directory.
     */
    inline virtual auto open_directory(const char *name) -> DirectoryBase
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
        return DirectoryBase("");
    }

    /**
     * @brief Create a file.
     * @param name Create a file with this name.
     * @param mode The mode to open the file in.
     * @return The file.
     */
    virtual auto create_file(const char *name, const char *mode)
        -> UniquePtr<FileBase>
    {
        (void)name;
        (void)mode;
        cf_assert(false, "Unimplemented base function called.");
        return UniquePtr<FileBase>(nullptr, stack_allocator);
    }

    /**
     * @brief Delete a file.
     * @param name The name of the file to delete.
     */
    virtual auto delete_file(const char *name) -> void
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
    }

    /**
     * @brief Check if a file exists.
     * @param name The name of the file.
     * @return True if the file exists, false otherwise.
     */
    virtual auto exists(const char *name) -> bool
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
        return false;
    }

    /**
     * @brief Check if a file is a directory.
     * @param name The name of the file.
     * @return True if the file is a directory, false otherwise.
     */
    virtual auto is_directory(const char *name) -> bool
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
        return false;
    }

    /**
     * @brief Create a directory.
     * @param name The name of the directory to create.
     */
    virtual auto create_directory(const char *name) -> void
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
    }

    /**
     * @brief Delete a directory.
     * @param name The name of the directory to delete.
     */
    virtual auto delete_directory(const char *name) -> void
    {
        (void)name;
        cf_assert(false, "Unimplemented base function called.");
    }
};

/**
 * @brief File is a factory class for File objects.
 */
class DirectoryFactory {
public:
    /**
     * @brief Get the current working directory.
     * @return The current working directory.
     */
    static auto cwd() -> DirectoryBase &;

    /**
     * @brief Open a directory.
     * @param path The path to the directory (from current working directory).
     * @return The directory.
     */
    static auto open(const char *path) -> DirectoryBase;
};

}