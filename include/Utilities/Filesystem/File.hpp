#pragma once
#include "../Types.hpp"
#include "../IO.hpp"
#include <cassert>
#include <variant>

namespace CrossFire
{

/**
 * @brief File is an interface for file objects.
 */
class FileBase {
public:
    /**
     * @brief Construct a new File object.
     * @param filename Filename of the file.
     * @param mode The mode to open the file in.
     */
    FileBase(const char *filename, const char *mode)
    {
        (void)filename;
        (void)mode;
    };
    virtual ~FileBase() = default;

    /**
     * @brief Read data from the file.
     * @param buffer The buffer to read into.
     * @return The number of bytes read.
     */
    virtual auto read(Slice<u8> &buffer) -> usize
    {
        (void)buffer;
        assert(false && "Unimplemented base function called.");
        return -1;
    }

    /**
     * @brief Write data to the file.
     * @param buffer The buffer to write from.
     * @return The number of bytes written.
     */
    virtual auto write(const Slice<u8> &buffer) -> usize
    {
        (void)buffer;
        assert(false && "Unimplemented base function called.");
        return -1;
    }

    /**
     * @brief Get the size of the file.
     * @return The size of the file.
     */
    virtual auto size() -> isize
    {
        assert(false && "Unimplemented base function called.");
        return -1;
    }

    /**
     * @brief Flush the file.
     * @return The number of bytes flushed.
     */
    virtual auto flush() -> void
    {
        assert(false && "Unimplemented base function called.");
    }

    /**
     * @brief Close the file.
     * @return The number of bytes flushed.
     */
    virtual auto close() -> void
    {
        assert(false && "Unimplemented base function called.");
    }

    /**
     * @brief Gets a reader for the file.
     * @return The reader.
     */
    auto reader() -> Reader
    {
        auto read = [](void *context, Slice<u8> &buffer) -> usize {
            return static_cast<FileBase *>(context)->read(buffer);
        };

        return { this, read };
    }

    /**
     * @brief Gets a writer for the file.
     * @return The writer.
     */
    auto writer() -> Writer
    {
        auto write = [](void *context, const Slice<u8> &buffer) -> usize {
            return static_cast<FileBase *>(context)->write(buffer);
        };
        return { this, write };
    }

protected:
    void *ctx = nullptr;
};

template <typename T> class UniquePtr;

/**
 * @brief FileFactory is a factory for file objects.
 */
class FileFactory {
public:
    /**
     * @brief Opens a file.
     * @param filename Filename of the file.
     * @param mode The mode to open the file in.
     * @return The file or nullptr if it failed to open.
     */
    static auto open(const char *filename, const char *mode)
        -> UniquePtr<FileBase>;

    /**
     * @brief Gets the standard output file.
     * @return The standard output file.
     */
    static auto get_stdout() -> FileBase *;

    /**
     * @brief Gets the standard error file.
     * @return The standard error file.
     */
    static auto get_stderr() -> FileBase *;
};

}