#include "CFile.hpp"
#include <Utilities/Allocator.hpp>
#include <cstdio>
#include <iostream>

namespace CrossFire::detail
{

CFile::CFile(const char *filename, const char *mode)
    : FileBase(filename, mode)
{
    PROFILE_ZONE;
    ctx = fopen(filename, mode);

    if (ctx == nullptr) {
        throw std::runtime_error("Failed to open file.");
    }
}

CFile::~CFile()
{
    PROFILE_ZONE;
    close();
}

CFile::CFile(FILE *file)
    : FileBase("", "")
{
    PROFILE_ZONE;
    ctx = file;
}

auto CFile::read(Slice<u8> &buffer) -> usize
{
    PROFILE_ZONE;
    return fread(buffer.ptr, 1, buffer.len, static_cast<FILE *>(ctx));
}

auto CFile::write(const Slice<u8> &buffer) -> usize
{
    PROFILE_ZONE;
    return fwrite(buffer.ptr, 1, buffer.len, static_cast<FILE *>(ctx));
}

auto CFile::size() -> isize
{
    PROFILE_ZONE;
    i32 pos = ftell(static_cast<FILE *>(ctx));
    fseek(static_cast<FILE *>(ctx), 0, SEEK_END);
    isize size = ftell(static_cast<FILE *>(ctx));
    fseek(static_cast<FILE *>(ctx), pos, SEEK_SET);
    return size;
}

auto CFile::flush() -> void
{
    PROFILE_ZONE;
    fflush(static_cast<FILE *>(ctx));
}

auto CFile::close() -> void
{
    PROFILE_ZONE;
    if (static_cast<FILE *>(ctx) != nullptr) {
        fclose(static_cast<FILE *>(ctx));
        ctx = nullptr;
    }
}

}

namespace CrossFire
{

auto FileFactory::open(const char *filename, const char *mode)
    -> UniquePtr<FileBase>
{
    PROFILE_ZONE;
    auto r = stack_allocator.create<detail::CFile>(filename, mode);
    FileBase *ptr = nullptr;
    if (r.is_ok()) {
        ptr = r.unwrap();
    }

    return UniquePtr<FileBase>(ptr, stack_allocator);
}

auto FileFactory::get_stdout() -> FileBase *
{
    PROFILE_ZONE;
    static auto stdout_file = detail::CFile(stdout);
    return &stdout_file;
}

auto FileFactory::get_stderr() -> FileBase *
{
    PROFILE_ZONE;
    static auto stderr_file = detail::CFile(stderr);
    return &stderr_file;
}

}