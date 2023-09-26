#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <stdexcept>

namespace CrossFire
{

inline auto cf_assert(bool condition, const char *message = nullptr) -> void;

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
typedef const char *cstring;

template <typename T> struct Slice {
    Slice(T *ptr, usize len)
        : ptr(ptr)
        , len(len)
    {
    }

    static auto from_string(cstring str) -> Slice<u8>
    {
        return Slice<u8>{ (u8 *)str, strlen(str) };
    }

    auto as_bytes() -> Slice<const u8>
    {
        return Slice<const u8>{ (const u8 *)ptr, len };
    }

    T *ptr;
    usize len;

    T &operator[](usize index)
    {
        cf_assert(index < len, "Slice index out of range");
        return ptr[index];
    }
};

}