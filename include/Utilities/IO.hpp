#pragma once
#include <utility>

#include "Types.hpp"

namespace CrossFire
{

// Function pointer read
typedef usize (*ReadFn)(void *ctx, Slice<u8> &buffer);
typedef usize (*WriteFn)(void *ctx, const Slice<u8> &buffer);

struct Reader {
	void *context = nullptr;
	ReadFn readFn = nullptr;

	Reader() = default;
	Reader(void *context, ReadFn readFn)
		: context(context)
		, readFn(readFn)
	{
	}

	[[nodiscard]] inline virtual auto raw_read(Slice<u8> &buffer) -> usize
	{
		return readFn(context, buffer);
	}

	template <typename T> inline auto read(T &value) -> usize
	{
		Slice<u8> buffer = Slice<u8>((u8 *)&value, sizeof(T));
		return raw_read(buffer);
	}

	template <typename T> inline auto readForeign(T &value) -> usize
	{
		Slice<u8> buffer = Slice<u8>((u8 *)&value, sizeof(T));
		usize read = raw_read(buffer);

		for (usize i = 0; i < sizeof(T) / 2; i++) {
			u8 tmp = buffer[i];
			buffer[i] = buffer[buffer.len - i - 1];
			buffer[buffer.len - i - 1] = tmp;
		}

		return read;
	}
};

struct Writer {
	void *context = nullptr;
	WriteFn writeFn = nullptr;

	Writer() = default;
	Writer(void *context, WriteFn writeFn)
		: context(context)
		, writeFn(writeFn)
	{
	}

	[[nodiscard]] inline virtual auto raw_write(const Slice<u8> &buffer)
		-> usize
	{
		return writeFn(context, buffer);
	}

	template <typename T> inline auto write(const T &value) -> usize
	{
		Slice<u8> buffer = Slice<u8>((u8 *)&value, sizeof(T));
		return raw_write(buffer);
	}

	template <typename T> inline auto writeForeign(const T &value) -> usize
	{
		T temp = value;
		Slice<u8> buffer = Slice<u8>((u8 *)&temp, sizeof(T));

		for (usize i = 0; i < sizeof(T) / 2; i++) {
			u8 tmp = buffer[i];
			buffer[i] = buffer[buffer.len - i - 1];
			buffer[buffer.len - i - 1] = tmp;
		}

		return raw_write(buffer);
	}
};

template <> inline auto Writer::write(const std::string &value) -> usize
{
	Slice<u8> buffer = Slice<u8>((u8 *)value.c_str(), value.length());
	return raw_write(buffer);
}

template <> inline auto Writer::write(const cstring &value) -> usize
{
	Slice<u8> buffer = Slice<u8>((u8 *)value, strlen(value));
	return raw_write(buffer);
}

struct BufferedReader : Reader {
	Reader reader;
	static constexpr usize size = 1024;
	u8 data[size] = { 0 };
	Slice<u8> buf;
	usize pos;
	usize len;

	explicit BufferedReader(Reader reader)
		: reader(std::move(reader))
		, buf(data, size)
		, pos(0)
		, len(0)
	{
	}

	[[nodiscard]] inline auto raw_read(Slice<u8> &buffer) -> usize override
	{
		usize read = 0;

		if (pos >= len) {
			read = reader.raw_read(this->buf);
			pos = 0;
			len = read;
		}

		if (read > 0) {
			usize toRead = buffer.len < len - pos ? buffer.len :
								len - pos;
			memcpy(buffer.ptr, this->buf.ptr + pos, toRead);
			pos += toRead;
			return toRead;
		}

		return 0;
	}
};

struct BufferedWriter : Writer {
	Writer writer;
	static constexpr usize size = 4096;
	u8 data[size] = { 0 };
	Slice<u8> buf;
	usize pos;

	explicit BufferedWriter(Writer writer)
		: writer(std::move(writer))
		, buf(data, size)
		, pos(0)
	{
	}

	[[nodiscard]] inline auto raw_write(const Slice<u8> &buffer)
		-> usize override
	{
		usize written = 0;

		if (pos + buffer.len > size) {
			written = writer.raw_write(Slice<u8>(buf.ptr, pos));
			pos = 0;
		}

		if (buffer.len > size) {
			written += writer.raw_write(buffer);
		} else {
			memcpy(buf.ptr + pos, buffer.ptr, buffer.len);
			pos += buffer.len;
			written += buffer.len;
		}

		return written;
	}

	inline auto flush() -> void
	{
		if (pos > 0) {
			(void)writer.raw_write(Slice<u8>(buf.ptr, pos));
			pos = 0;
		}
	}
};

}