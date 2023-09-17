#include "CFile.hpp"

namespace CrossFire {

CFile::CFile(const char* filename, const char* mode) : IFile(filename, mode) {
	file = fopen(filename, mode);

	if (file == nullptr) {
		throw std::runtime_error("Failed to open file.");
	}
}
CFile::CFile(FILE* file) : IFile("", "") {
	this->file = file;
}

CFile::~CFile() {
	fclose(file);
}

auto CFile::read(Slice<u8>& buffer) -> usize {
	return fread(buffer.ptr, 1, buffer.len, file);
}

auto CFile::write(const Slice<u8>& buffer) -> usize {
	return fwrite(buffer.ptr, 1, buffer.len, file);
}

auto CFile::size() -> isize {
	i32 pos = ftell(file);
	fseek(file, 0, SEEK_END);
	isize size = ftell(file);
	fseek(file, pos, SEEK_SET);
	return size;
}

auto CFile::flush() -> void {
	fflush(file);
}

auto CFile::close() -> void {
	if (file != nullptr) {
		fclose(file);
		file = nullptr;
	}
}

}