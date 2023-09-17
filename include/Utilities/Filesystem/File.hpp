#pragma once
#include "../Types.hpp"
#include <variant>

namespace CrossFire {

/**
 * @brief IFile is an interface for file objects.
 */
class IFile {
    public:
    	IFile(const char* filename, const char* mode) : filename(filename), mode(mode) {};
	virtual ~IFile() = default;

	virtual auto read(Slice<u8>& buffer) -> usize = 0;
	virtual auto write(const Slice<u8>& buffer) -> usize = 0;
	virtual auto size() -> isize = 0;
	virtual auto flush() -> void = 0;
	virtual auto close() -> void = 0;

    protected:
	const char* filename;
	const char* mode;
};

/**
 * @brief File is a factory class for IFile objects.
 */
class File {
    public:
	/**
	 * @brief open opens a file.
	 * @param filename Filename to open.
	 * @param mode Mode to open the file in.
	 * @return IFile object.
	 * @throws std::runtime_error if the file could not be opened.
	 */
	static auto open(const char* filename, const char* mode) -> IFile*;

	static auto get_stdout() -> IFile*;
	static auto get_stderr() -> IFile*;
};

}