#pragma once
#include <Utilities/Filesystem/File.hpp>
#include <cstdio>

namespace CrossFire::detail
{

/**
 * @brief File is an interface for file objects.
 */
class CFile final : public FileBase {
public:
    CFile(const char *filename, const char *mode);
    explicit CFile(FILE *file);
    ~CFile() override;

    auto read(Slice<u8> &buffer) -> usize override;
    auto write(const Slice<u8> &buffer) -> usize override;
    auto size() -> isize override;

    auto flush() -> void override;
    auto close() -> void override;
};

}