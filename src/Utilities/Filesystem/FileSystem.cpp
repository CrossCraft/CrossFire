#include <Utilities/Filesystem/Filesystem.hpp>

namespace CrossFire
{

auto FileSystem::cwd() -> DirectoryBase &
{
    return DirectoryFactory::cwd();
}

auto FileSystem::get_stdout() -> FileBase *
{
    return FileFactory::get_stdout();
}

auto FileSystem::get_stderr() -> FileBase *
{
    return FileFactory::get_stderr();
}

}