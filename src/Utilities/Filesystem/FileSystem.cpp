#include <Utilities/Filesystem/Filesystem.hpp>

namespace CrossFire
{

auto FileSystem::cwd() -> DirectoryBase &
{
    PROFILE_ZONE;
    return DirectoryFactory::cwd();
}

auto FileSystem::get_stdout() -> FileBase *
{
    PROFILE_ZONE;
    return FileFactory::get_stdout();
}

auto FileSystem::get_stderr() -> FileBase *
{
    PROFILE_ZONE;
    return FileFactory::get_stderr();
}

}