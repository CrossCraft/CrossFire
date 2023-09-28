#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <variant>
#include <optional>

namespace CrossFire
{

auto cf_assert(bool condition, const char *message = nullptr) -> void;

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
    Slice()
        : ptr(nullptr)
        , len(0)
    {
    }

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

template <typename... T> using TaggedUnion = std::variant<T...>;
template <typename T> using Option = std::optional<T>;
using None = std::nullopt_t;
using Ok = std::monostate;

template <typename T, typename E> class Result {
    TaggedUnion<T, E> value;

public:
    // Not explicit to allow implicit return
    Result(T value)
        : value(value)
    {
    }

    // Not explicit to allow implicit return
    Result(E value)
        : value(value)
    {
    }

    auto is_ok() -> bool
    {
        return std::holds_alternative<T>(value);
    }

    auto is_err() -> bool
    {
        return std::holds_alternative<E>(value);
    }

    auto unwrap() -> T
    {
        cf_assert(is_ok(), "Result is not ok");

        return std::get<T>(value);
    }

    auto unwrap_err() -> E
    {
        cf_assert(is_err(), "Result is not err");

        return std::get<E>(value);
    }
};
template <typename E> using Error = Result<Ok, E>;

}