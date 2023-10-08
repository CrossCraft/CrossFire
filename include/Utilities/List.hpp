#pragma once
#include "Allocator.hpp"

namespace CrossFire
{

/**
 * @brief A dynamic array.
 * @tparam T The type of the elements.
 */
template <typename T> class List {
    Allocator &allocator;

public:
    /**
     * @brief Creates a new list.
     * @param allocator The allocator to use.
     */
    List(Allocator &allocator)
        : allocator(allocator)
        , data(allocator.alloc<T>(8).unwrap())
        , capacity(8)
    {
        PROFILE_ZONE;
        data.len = 0;
    }

    /**
     * @brief Adds an element to the list.
     * @param element The element to add.
     * @return The new list -- or an error if allocation failed.
     */
    inline auto push(const T &element) -> ResultVoid<AllocationError>
    {
        PROFILE_ZONE;
        if (data.len == capacity) {
            // Grow the list
            auto new_capacity = capacity * 2;

            // Reallocate the list
            auto new_data = allocator.realloc(data, new_capacity);
            if (new_data.is_err())
                return new_data.unwrap_err();

            auto curr_len = data.len;
            data = new_data.unwrap();
            data.len = curr_len;

            capacity = new_capacity;
        }

        // Grow the list by one
        data.len += 1;

        // Move the last element to the end of the list
        data.back() = element;

        return Ok();
    }

    /**
     * @brief Removes the last element from the list
     */
    inline auto pop() -> void
    {
        PROFILE_ZONE;
        data.len -= 1;
    }

    /**
     * @brief Clears the list.
     */
    inline auto clear() -> void
    {
        PROFILE_ZONE;
        data.len = 0;
    }

    /**
     * @brief Reserves the given capacity for the list.
     * @param new_capacity The new capacity.
     * @return The new list -- or an error if allocation failed.
     */
    inline auto reserve(usize new_capacity) -> ResultVoid<AllocationError>
    {
        PROFILE_ZONE;
        if (new_capacity > capacity) {
            // Reallocate the list
            auto new_data = allocator.reallocate(data, new_capacity);
            if (new_data.is_err())
                return new_data.unwrap_err();

            data = new_data.unwrap();
            capacity = new_capacity;
        }

        return Ok();
    }

    /**
     * @brief Shrinks the list to fit its current size.
     * @return The new list -- or an error if allocation failed.
     */
    inline auto shrink_to_fit() -> ResultVoid<AllocationError>
    {
        PROFILE_ZONE;
        if (data.len < capacity) {
            // Reallocate the list
            auto new_data = allocator.reallocate(data, data.len);
            if (new_data.is_ok()) {
                data = new_data.unwrap();
                capacity = data.len;
            } else {
                return new_data.unwrap_err();
            }
        }

        return Ok();
    }

    /**
     * @brief Gets the element at the given index.
     * @param index The index of the element.
     * @return The element.
     */
    inline auto operator[](usize index) -> T &
    {
        PROFILE_ZONE;
        return data[index];
    }

    /**
     * @brief Gets the last element of the list.
     * @return The last element.
     */
    inline auto back() -> T &
    {
        PROFILE_ZONE;
        return data[data.len - 1];
    }

    Slice<T> data;
    usize capacity;

    /**
     * @brief Destroys the list.
     */
    ~List()
    {
        PROFILE_ZONE;
        allocator.dealloc<T>(Slice<T>(data.ptr, capacity));
    }
};

template <typename T> using Stack = List<T>;

}