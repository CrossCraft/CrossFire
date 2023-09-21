#include <Utilities/Filesystem/File.hpp>
#include <cstdio>

namespace CrossFire {


File::File(const char* filename, const char* mode) {
	ctx = fopen(filename, mode);

	if (ctx == nullptr) {
		throw std::runtime_error("Failed to open file.");
	}
}

File::File(FILE* file) {
	ctx = file;
}

auto File::read(Slice<u8>& buffer) -> usize {
	return fread(buffer.ptr, 1, buffer.len, static_cast<FILE*>(ctx));
}

auto File::write(const Slice<u8>& buffer) -> usize {
	return fwrite(buffer.ptr, 1, buffer.len, static_cast<FILE*>(ctx));
}

auto File::size() -> isize {
	i32 pos = ftell(static_cast<FILE*>(ctx));
	fseek(static_cast<FILE*>(ctx), 0, SEEK_END);
	isize size = ftell(static_cast<FILE*>(ctx));
	fseek(static_cast<FILE*>(ctx), pos, SEEK_SET);
	return size;
}

auto File::flush() -> void {
	fflush(static_cast<FILE*>(ctx));
}

auto File::close() -> void {
	if (static_cast<FILE*>(ctx) != nullptr) {
		fclose(static_cast<FILE*>(ctx));
		ctx = nullptr;
	}
}

auto FileFactory::open(const char* filename, const char* mode) -> File
{
	return {filename, mode};
}

auto FileFactory::get_stdout() -> File& {
	static auto stdout_file = File(stdout);
	return stdout_file;
}

auto FileFactory::get_stderr() -> File& {
	static auto stderr_file = File(stderr);
	return stderr_file;
}

}