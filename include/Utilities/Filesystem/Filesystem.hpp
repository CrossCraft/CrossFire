#include "File.hpp"
#include "Directory.hpp"

namespace CrossFire
{

/**
 * @brief File is a factory class for File objects.
 */
class FileSystem final {
public:
    static auto cwd() -> DirectoryBase &;

    static auto get_stdout() -> FileBase *;
    static auto get_stderr() -> FileBase *;
};

}