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
    FileBase(const char *filename, const char *mode)
    {
        (void)filename;
        (void)mode;
    };
    virtual ~FileBase() = default;

    virtual auto read(Slice<u8> &buffer) -> usize
    {
        (void)buffer;
        assert(false && "Unimplemented base function called.");
        return -1;
    }
    virtual auto write(const Slice<u8> &buffer) -> usize
    {
        (void)buffer;
        assert(false && "Unimplemented base function called.");
        return -1;
    }
    virtual auto size() -> isize
    {
        assert(false && "Unimplemented base function called.");
        return -1;
    }

    virtual auto flush() -> void
    {
        assert(false && "Unimplemented base function called.");
    }
    virtual auto close() -> void
    {
        assert(false && "Unimplemented base function called.");
    }

    auto reader() -> Reader
    {
        auto read = [](void *context, Slice<u8> &buffer) -> usize {
            return static_cast<FileBase *>(context)->read(buffer);
        };

        return { this, read };
    }

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

/**
 * @brief File is a factory class for File objects.
 */
class FileFactory {
public:
    /**
	 * @brief open opens a file.
	 * @param filename Filename to open.
	 * @param mode Mode to open the file in.
	 * @return File object.
	 * @throws std::runtime_error if the file could not be opened.
	 */
    static auto open(const char *filename, const char *mode) -> FileBase;

    static auto get_stdout() -> FileBase &;
    static auto get_stderr() -> FileBase &;
};

}