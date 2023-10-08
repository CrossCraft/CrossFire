#pragma once
#include <array>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <map>
#include <optional>
#include <stdexcept>
#include <variant>
#include "Profiler.hpp"

namespace CrossFire
{

/**
 * @brief Asserts that a condition is true.
 * @param condition Condition to assert.
 * @param message Message to print if the condition is false.
 */
auto cf_assert(bool condition, const char *message = nullptr) -> void;

/**
 * Define default types
 */

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

/**
 * @brief Slice is a view into a contiguous array of elements.
 * @tparam T The type of the elements.
 */
template <typename T> struct Slice {
    /**
     * @brief Construct an empty slice.
     */
    Slice()
        : ptr(nullptr)
        , len(0)
    {
        PROFILE_ZONE;
    }

    /**
     * @brief Construct a slice from a pointer and a length.
     * @param ptr Pointer to the first element.
     * @param len Length of the slice.
     */
    Slice(T *ptr, usize len)
        : ptr(ptr)
        , len(len)
    {
        PROFILE_ZONE;
    }

    /**
     * @brief Construct a C string.
     * @param str Pointer to the first element.
     * @return The slice.
     */
    static auto from_string(cstring str) -> Slice<u8>
    {
        PROFILE_ZONE;
        return Slice<u8>{ (u8 *)str, strlen(str) };
    }

    /**
     * @brief Make this slice into bytes
     * @return The slice as bytes.
     */
    auto as_bytes() -> Slice<const u8>
    {
        PROFILE_ZONE;
        return Slice<const u8>{ (const u8 *)ptr, len * sizeof(T) };
    }

    /**
     * @brief Get the last element of the slice.
     * @return The last element.
     */
    auto back() -> T &
    {
        PROFILE_ZONE;
        cf_assert(len > 0, "Slice is empty");
        return ptr[len - 1];
    }

    T *ptr;
    usize len;

    /**
     * @brief Get the first element of the slice.
     * @param index The index of the element.
     * @return The element.
     */
    T &operator[](usize index)
    {
        PROFILE_ZONE;
        cf_assert(index < len, "Slice index out of range");
        return ptr[index];
    }
};

/**
 * @brief TaggedUnion is a union that can hold any of the given types.
 */
template <typename... T> using TaggedUnion = std::variant<T...>;

/**
 * @brief Option is a type that can either hold a value or nothing.
 */
template <typename T> using Option = std::optional<T>;

/**
 * @brief None is a type that represents nothing.
 */
using None = std::nullopt_t;

/**
 * @brief Ok is a type that represents success in a Result.
 */
using Ok = std::monostate;

/**
 * @brief Result is a type that can either hold a value or an error.
 * @tparam T The type of the value.
 * @tparam E The type of the error.
 */
template <typename T, typename E> class Result {
    TaggedUnion<T, E> value;

public:
    // Not explicit to allow implicit return (preferred)
    Result(T value)
        : value(value)
    {
        PROFILE_ZONE;
    }

    // Not explicit to allow implicit return (preferred)
    Result(E value)
        : value(value)
    {
        PROFILE_ZONE;
    }

    // Delete copy
    Result(const Result &other) = delete;
    auto operator=(const Result &other) -> Result & = delete;

    // Move
    Result(Result &&other) noexcept
        : value(std::move(other.value))
    {
        PROFILE_ZONE;
    }

    auto operator=(Result &&other) noexcept -> Result &
    {
        PROFILE_ZONE;
        value = std::move(other.value);
        return *this;
    }

    /**
     * @brief Check if the result is ok.
     * @return True if the result is ok, false otherwise.
     */
    auto is_ok() -> bool
    {
        PROFILE_ZONE;
        return std::holds_alternative<T>(value);
    }

    /**
     * @brief Check if the result is an error.
     * @return True if the result is an error, false otherwise.
     */
    auto is_err() -> bool
    {
        PROFILE_ZONE;
        return std::holds_alternative<E>(value);
    }

    /**
     * @brief Unwrap the result.
     * @return The value if the result is ok.
     */
    auto unwrap() -> T
    {
        PROFILE_ZONE;
        cf_assert(is_ok(), "Result is not ok");

        return std::get<T>(value);
    }

    /**
     * @brief Unwrap the result.
     * @return The error if the result is an error.
     */
    auto unwrap_err() -> E
    {
        PROFILE_ZONE;
        cf_assert(is_err(), "Result is not err");

        return std::get<E>(value);
    }
};

/**
 * @brief ResultVoid is a result that either holds Ok or an error.
 */
template <typename E> using ResultVoid = Result<Ok, E>;

/**
 * @brief Array is a fixed-size array.
 */
template <typename T, usize N> using Array = std::array<T, N>;

}