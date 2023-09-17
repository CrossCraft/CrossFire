#include <Utilities/Filesystem/File.hpp>
#include "CFile.hpp"
#include <cstdio>

namespace CrossFire {

auto File::open(const char* filename, const char* mode) -> IFile*
{
	return new CFile(filename, mode);
}

auto File::get_stdout() -> IFile* {
	return new CFile(stdout);
}

auto File::get_stderr() -> IFile* {
	return new CFile(stderr);
}

}