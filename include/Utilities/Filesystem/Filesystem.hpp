#include "File.hpp"
#include "Directory.hpp"

namespace CrossFire
{

/**
 * @brief File is a factory class for File objects.
 */
class FileSystem final {
public:
    /**
     * @brief Get the current working directory.
     * @return The current working directory.
     */
    static auto cwd() -> DirectoryBase &;

    /**
     * @brief Get the standard output file.
     * @return The standard output file.
     */
    static auto get_stdout() -> FileBase *;

    /**
     * @brief Get the standard error file.
     * @return The standard error file.
     */
    static auto get_stderr() -> FileBase *;
};

}