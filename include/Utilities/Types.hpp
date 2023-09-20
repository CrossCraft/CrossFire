#pragma once

#include <cstdint>
#include <stdexcept>

namespace CrossFire
{

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef size_t usize;
typedef ptrdiff_t isize;

template <typename T> struct Slice {
	Slice(T *ptr, usize len) : ptr(ptr), len(len) {}

	static auto from_string(const char *str) -> Slice<u8>
	{
		return Slice<u8>{(u8*)str, strlen(str)};
	}

	auto as_bytes() -> Slice<const u8>
	{
		return Slice<const u8>{(const u8*)ptr, len};
	}

	T *ptr;
	usize len;

	T &operator[](usize index)
	{
#ifdef NDEBUG
		if (index >= len) {
			throw std::out_of_range("Slice index out of range");
		}
#endif

		return ptr[index];
	}
};

}