#pragma once
#include "../Types.hpp"
#include <variant>

namespace CrossFire {

/**
 * @brief File is an interface for file objects.
 */
class File {
    public:
	File() = default;
	explicit File(FILE* file);
	File(const char* filename, const char* mode);
	virtual ~File() = default;

	virtual auto read(Slice<u8>& buffer) -> usize;
	virtual auto write(const Slice<u8>& buffer) -> usize;
	virtual auto size() -> isize;
	virtual auto flush() -> void;
	virtual auto close() -> void;

    protected:
	void* ctx = nullptr;
};

/**
 * @brief File is a factory class for File objects.
 */
class FileFactory {
    public:
	/**
	 * @brief open opens a file.
	 * @param filename Filename to open.
	 * @param mode Mode to open the file in.
	 * @return File object.
	 * @throws std::runtime_error if the file could not be opened.
	 */
	static auto open(const char* filename, const char* mode) -> File;

	static auto get_stdout() -> File;
	static auto get_stderr() -> File;
};

}