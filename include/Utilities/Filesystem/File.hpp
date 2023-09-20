#pragma once
#include "../Types.hpp"
#include "../IO.hpp"
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

	auto reader() -> Reader {
		return Reader {
			.context = this,
			.readFn = [](void* context, Slice<u8>& buffer) -> usize {
				return static_cast<File*>(context)->read(buffer);
			}
		};
	}

	auto writer() -> Writer {
		return Writer {
			.context = this,
			.writeFn = [](void* context, const Slice<u8>& buffer) -> usize {
				return static_cast<File*>(context)->write(buffer);
			}
		};
	}

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