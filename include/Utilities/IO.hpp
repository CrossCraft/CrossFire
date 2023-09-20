#pragma once
#include "Types.hpp"

namespace CrossFire {

// Function pointer read
typedef usize (*ReadFn)(void* ctx, Slice<u8>& buffer);
typedef usize (*WriteFn)(void* ctx, const Slice<u8>& buffer);

struct Reader {
    void* context;
    ReadFn readFn;

    [[nodiscard]]
    auto raw_read(Slice<u8>& buffer) const -> usize {
	return readFn(context, buffer);
    }

    template<typename T>
    auto read(T& value) -> usize {
	Slice<u8> buffer = Slice<u8>((u8*)&value, sizeof(T));
	return raw_read(buffer);
    }

    template<typename T>
    auto readForeign(T& value) -> usize {
	Slice<u8> buffer = Slice<u8>((u8*)&value, sizeof(T));
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
    void* context;
    WriteFn writeFn;

    [[nodiscard]]
    auto raw_write(const Slice<u8>& buffer) const -> usize {
	return writeFn(context, buffer);
    }

    template<typename T>
    auto write(const T& value) -> usize {
	Slice<u8> buffer = Slice<u8>((u8*)&value, sizeof(T));
	return raw_write(buffer);
    }

    template<>
    auto write(const std::string& value) -> usize {
	Slice<u8> buffer = Slice<u8>((u8*)value.c_str(), value.length());
	return raw_write(buffer);
    }

    template<typename T>
    auto writeForeign(const T& value) -> usize {
	T temp = value;
	Slice<u8> buffer = Slice<u8>((u8*)&temp, sizeof(T));

	for (usize i = 0; i < sizeof(T) / 2; i++) {
	    u8 tmp = buffer[i];
	    buffer[i] = buffer[buffer.len - i - 1];
	    buffer[buffer.len - i - 1] = tmp;
	}

	return raw_write(buffer);
    }
};

}