#pragma once
#include <cstdio>
#include <Utilities/Types.hpp>
#include <Utilities/Filesystem/File.hpp>

namespace CrossFire {

class CFile : public IFile {
    public:
	CFile(const char* filename, const char* mode);
	CFile(FILE* file);
	~CFile() override;

	auto read(Slice<u8>& buffer) -> usize override;
	auto write(const Slice<u8>& buffer) -> usize override;
	auto size() -> isize override;
	auto flush() -> void override;
	auto close() -> void override;

    private:
	FILE* file;
};

}