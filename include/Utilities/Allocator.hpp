#pragma once
#include <map>
#include <string>
#include "Types.hpp"
#include "Logger.hpp"

namespace CrossFire
{

enum class AllocationError {
    OutOfMemory,
    ReallocFailed,
    InvalidSize,
};

/**
 * @brief The Allocator struct is the base class for all allocators.
 * It provides a simple interface for allocating, deallocating and reallocating
 * memory.
 */
struct Allocator {
    virtual ~Allocator() = default;

    /**
     * @brief Allocate memory.
     * @param size The size of the memory to allocate.
     * @param alignment The alignment of the memory to allocate.
     * @return A slice to the allocated memory.
     */
    [[nodiscard]] virtual auto
    allocate(usize size, usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> = 0;

    /**
     * @brief Deallocate memory.
     * @param ptr The pointer to the memory to deallocate.
     */
    virtual auto deallocate(Slice<u8> ptr) -> void = 0;

    /**
     * @brief Reallocate memory.
     * @param ptr  The pointer to the memory to reallocate.
     * @param size The new size of the memory.
     * @param alignment The alignment of the memory to reallocate.
     * @return A slice to the reallocated memory.
     */
    [[nodiscard]] virtual auto
    reallocate(Slice<u8> ptr, usize size,
               usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> = 0;

    /**
     * @brief Create an object.
     * @tparam T The type of the object.
     * @tparam Args The types of the arguments.
     * @param args The arguments to pass to the constructor.
     * @return A pointer to the created object.
     */
    template <typename T, typename... Args>
    [[nodiscard]] auto create(Args &&...args) -> Result<T *, AllocationError>
    {
        auto ptr = allocate(sizeof(T), alignof(T));
        if (ptr.is_err())
            return ptr.unwrap_err();

        return new (ptr.unwrap().ptr) T(std::forward<Args>(args)...);
    }

    /**
     * @brief Destroy an object.
     * @tparam T The type of the object.
     * @param ptr The pointer to the object.
     */
    template <typename T> auto destroy(T *ptr) -> void
    {
        ptr->~T();
        deallocate(Slice<u8>((u8 *)ptr, sizeof(T)));
    }

    /**
     * @brief Allocate a slice of memory.
     * @tparam T The type of the slice.
     * @param count The number of elements in the slice.
     * @return A slice to the allocated memory.
     */
    template <typename T>
    [[nodiscard]] auto alloc(usize count) -> Result<Slice<T>, AllocationError>
    {
        auto ptr = allocate(sizeof(T) * count, alignof(T));
        if (ptr.is_err())
            return ptr.unwrap_err();

        return Slice<T>((T *)(ptr.unwrap().ptr), count);
    }

    /**
     * @brief Reallocate a slice of memory.
     * @tparam T The type of the slice.
     * @param ptr The pointer to the slice.
     * @param count The new number of elements in the slice.
     * @return A slice to the reallocated memory.
     */
    template <typename T>
    [[nodiscard]] auto realloc(Slice<T> ptr, usize count)
        -> Result<Slice<T>, AllocationError>
    {
        auto nptr = reallocate(Slice<u8>((u8 *)ptr.ptr, sizeof(T) * ptr.len),
                               sizeof(T) * count, alignof(T));
        if (nptr.is_err())
            return nptr.unwrap_err();

        return Slice<T>((T *)(nptr.unwrap().ptr), count);
    }

    /**
     * @brief Deallocate a slice of memory.
     * @tparam T The type of the slice.
     * @param slice The slice to deallocate.
     */
    template <typename T> auto dealloc(Slice<T> slice) -> void
    {
        deallocate(Slice<u8>((u8 *)slice.ptr, sizeof(T) * slice.len));
    }
};

/**
 * @brief The CAllocator struct is an allocator that uses the C functions aligned_alloc, realloc and free.
 */
struct CAllocator final : public Allocator {
    CAllocator() = default;
    ~CAllocator() override = default;

    [[nodiscard]] auto allocate(usize size,
                                usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
    auto deallocate(Slice<u8> ptr) -> void override;
    [[nodiscard]] auto reallocate(Slice<u8> ptr, usize size,
                                  usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
};

extern CAllocator c_allocator;

/**
 * @brief The LinearAllocator struct is an allocator that allocates memory linearly.
 * It does not support deallocation or reallocation.
 * All allocations are freed when the allocator is destroyed.
 */
class LinearAllocator final : public Allocator {
    Slice<u8> memory;
    usize offset;
    Allocator &backing_allocator;

public:
    explicit LinearAllocator(usize size, Allocator &allocator = c_allocator);
    ~LinearAllocator() override;

    [[nodiscard]] auto allocate(usize size,
                                usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
    auto deallocate(Slice<u8> ptr) -> void override;
    [[nodiscard]] auto reallocate(Slice<u8> ptr, usize size,
                                  usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
};

/**
 * @brief The StackAllocator struct is an allocator that allocates memory linearly.
 * All allocations are freed when the allocator is destroyed.
 * You can deallocate memory, but only the last allocation.
 */
class StackAllocator : public Allocator {
    Slice<u8> memory;
    usize offset;
    usize last_offset;
    Allocator &backing_allocator;

public:
    explicit StackAllocator(usize size, Allocator &allocator = c_allocator);
    ~StackAllocator() override;

    [[nodiscard]] auto allocate(usize size,
                                usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
    auto deallocate(Slice<u8> ptr) -> void override;
    [[nodiscard]] auto reallocate(Slice<u8> ptr, usize size,
                                  usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
};

/**
 * @brief The DebugAllocator struct is an allocator that tracks allocations and deallocations.
 * It is useful for detecting memory leaks.
 * It is not recommended to use this allocator in production.
 * This allocator wraps another allocator.
 */
class DebugAllocator final : public Allocator {
    Allocator &backing_allocator;
    usize alloc_count = 0;
    usize dealloc_count = 0;
    usize alloc_size = 0;
    usize dealloc_size = 0;

    usize current_usage = 0;
    usize peak_usage = 0;

public:
    explicit DebugAllocator(Allocator &allocator = c_allocator)
        : backing_allocator(allocator)
    {
    }
    ~DebugAllocator() override
    {
        if (detect_leaks()) {
            auto &err = Logger::get_stderr();
            err.err("Memory leak detected!");
            err.err(("Allocated " + std::to_string(alloc_count) + " times.")
                        .c_str());
            err.err(("Deallocated " + std::to_string(dealloc_count) + " times.")
                        .c_str());
            err.err(
                ("Allocated " + std::to_string(alloc_size) + " bytes.").c_str());
            err.err(("Deallocated " + std::to_string(dealloc_size) + " bytes.")
                        .c_str());
            err.err(
                ("Current usage: " + std::to_string(current_usage) + " bytes.")
                    .c_str());
        }
    };

    [[nodiscard]] auto allocate(usize size,
                                usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
    auto deallocate(Slice<u8> ptr) -> void override;
    [[nodiscard]] auto reallocate(Slice<u8> ptr, usize size,
                                  usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;

    inline auto get_alloc_count() const -> usize
    {
        return alloc_count;
    }
    inline auto get_dealloc_count() const -> usize
    {
        return dealloc_count;
    }
    inline auto get_alloc_size() const -> usize
    {
        return alloc_size;
    }
    inline auto get_dealloc_size() const -> usize
    {
        return dealloc_size;
    }

    auto detect_leaks() -> bool
    {
        return alloc_count != dealloc_count || alloc_size != dealloc_size;
    }

    auto get_current_usage() const -> usize
    {
        return current_usage;
    }
    auto get_peak_usage() const -> usize
    {
        return peak_usage;
    }
};

/**
 * @brief The GPAllocator struct is an allocator that allocates memory for general purpose.
 * This allocator is efficient for allocating and deallocating memory, but not for reallocating.
 * This allocator takes in a slice of memory to use or allocates its own memory.
 * This allocator operates in O(log(n)) time for allocation and deallocation.
 */
class GPAllocator : public Allocator {
    struct Allocation {
        usize base;
        usize size;

        Allocation(usize base, usize size)
            : base(base)
            , size(size)
        {
        }
    };

    std::multimap<usize, Allocation> free_map;
    std::map<usize, Allocation> reserved_map;
    Slice<u8> memory;

    Allocator *backing_allocator;

public:
    explicit GPAllocator(Slice<u8> memory);
    explicit GPAllocator(usize size, Allocator *allocator = &c_allocator);
    ~GPAllocator() override;

    [[nodiscard]] auto allocate(usize size,
                                usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
    auto deallocate(Slice<u8> ptr) -> void override;
    [[nodiscard]] auto reallocate(Slice<u8> ptr, usize size,
                                  usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
};

extern GPAllocator stack_allocator;

/**
 * @brief The UniquePtr class is a smart pointer that owns and manages another object through a pointer and disposes of that object when the UniquePtr goes out of scope.
 * @tparam T The type of the object.
 */
template <typename T> class UniquePtr {
    T *ptr;
    Allocator &allocator;

public:
    explicit UniquePtr(T *ptr, Allocator &allocator)
        : ptr(ptr)
        , allocator(allocator)
    {
    }

    ~UniquePtr()
    {
        allocator.destroy(ptr);
    }

    UniquePtr(const UniquePtr<T> &other) = delete;
    UniquePtr<T> &operator=(const UniquePtr<T> &other) = delete;

    UniquePtr(UniquePtr<T> &&other) noexcept : ptr(other.ptr),
                                               allocator(other.allocator)
    {
        other.ptr = nullptr;
    }

    inline auto operator=(UniquePtr<T> &&other) noexcept->UniquePtr<T> &
    {
        if (this != &other) {
            allocator.destroy(ptr);
            ptr = other.ptr;
            allocator = other.allocator;
            other.ptr = nullptr;
        }
        return *this;
    }

    inline auto operator*() const -> T &
    {
        return *ptr;
    }

    inline auto operator->() const -> T *
    {
        return ptr;
    }

    inline auto get() const -> T *
    {
        return ptr;
    }

    inline auto reset(T *new_ptr) -> void
    {
        allocator.destroy(ptr);
        ptr = new_ptr;
    }

    inline auto swap(UniquePtr<T> &other) noexcept->void
    {
        T *tmp = ptr;
        ptr = other.ptr;
        other.ptr = tmp;
    }

    inline auto release() -> T *
    {
        T *tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    /**
     * @brief Create a unique pointer.
     * @tparam Args The types of the arguments.
     * @param allocator The allocator to use.
     * @param args The arguments to pass to the constructor.
     * @return The new unique pointer -- or an error if allocation failed.
     */
    template <typename... Args>
    inline static auto create(Allocator &allocator, Args &&...args)
        -> Result<UniquePtr<T>, AllocationError>
    {
        auto ptr = allocator.create<T>(std::forward<Args>(args)...);
        if (ptr.is_err())
            return ptr.unwrap_err();
        return UniquePtr<T>(ptr.unwrap(), allocator);
    }
};

/**
 * @brief Create a unique pointer on the stack.
 * @tparam T The type of the object.
 * @tparam Args The types of the arguments.
 * @param args The arguments to pass to the constructor.
 * @return The new unique pointer -- or an error if allocation failed.
 */
template <typename T, typename... Args>
inline auto create_unique_stack(Args &&...args)
    -> Result<UniquePtr<T>, AllocationError>
{
    return UniquePtr<T>::create(stack_allocator, std::forward<Args>(args)...);
}

/**
 * @brief The SharedPtr class is a smart pointer that retains shared ownership of an object through a pointer.
 * @tparam T The type of the object.
 */
template <typename T> class SharedPtr {
    T *ptr;
    usize *ref_count;
    Allocator &allocator;

public:
    explicit SharedPtr(T *ptr, Allocator &allocator)
        : ptr(ptr)
        , ref_count(allocator.create<usize>(1).unwrap())
        , allocator(allocator)
    {
    }

    ~SharedPtr()
    {
        if (--(*ref_count) == 0) {
            allocator.destroy(ref_count);
            allocator.destroy(ptr);
        }
    }

    SharedPtr(const SharedPtr<T> &other)
        : ptr(other.ptr)
        , ref_count(other.ref_count)
        , allocator(other.allocator)
    {
        ++(*ref_count);
    }

    inline auto operator=(const SharedPtr<T> &other) -> SharedPtr<T> &
    {
        if (this != &other) {
            if (--(*ref_count) == 0) {
                allocator.destroy(ref_count);
                allocator.destroy(ptr);
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            allocator = other.allocator;
            ++(*ref_count);
        }
        return *this;
    }

    SharedPtr(SharedPtr<T> &&other) noexcept : ptr(other.ptr),
                                               ref_count(other.ref_count),
                                               allocator(other.allocator)
    {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    inline auto operator=(SharedPtr<T> &&other) noexcept->SharedPtr<T> &
    {
        if (this != &other) {
            if (--(*ref_count) == 0) {
                allocator.destroy(ref_count);
                allocator.destroy(ptr);
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            allocator = other.allocator;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    inline auto operator*() const -> T &
    {
        return *ptr;
    }

    inline auto operator->() const -> T *
    {
        return ptr;
    }

    inline auto get() const -> T *
    {
        return ptr;
    }

    inline auto reset(T *new_ptr) -> void
    {
        if (--(*ref_count) == 0) {
            allocator.destroy(ref_count);
            allocator.destroy(ptr);
        }
        ptr = new_ptr;
        ref_count = allocator.create<usize>(1).unwrap();
    }

    inline auto swap(SharedPtr<T> &other) noexcept->void
    {
        T *tmp = ptr;
        ptr = other.ptr;
        other.ptr = tmp;
        usize *tmp2 = ref_count;
        ref_count = other.ref_count;
        other.ref_count = tmp2;
    }

    /**
     * @brief Create a shared pointer.
     * @tparam Args The types of the arguments.
     * @param allocator The allocator to use.
     * @param args The arguments to pass to the constructor.
     * @return The new shared pointer -- or an error if allocation failed.
     */
    template <typename... Args>
    inline static auto create(Allocator &allocator, Args &&...args)
        -> Result<SharedPtr<T>, AllocationError>
    {
        auto ptr = allocator.create<T>(std::forward<Args>(args)...);
        if (ptr.is_err())
            return ptr.unwrap_err();
        return SharedPtr<T>(ptr.unwrap(), allocator);
    }
};

/**
 * @brief Create a shared pointer on the stack.
 * @tparam T The type of the object.
 * @tparam Args The types of the arguments.
 * @param args The arguments to pass to the constructor.
 * @return The new shared pointer -- or an error if allocation failed.
 */
template <typename T, typename... Args>
inline auto create_shared_stack(Args &&...args)
    -> Result<SharedPtr<T>, AllocationError>
{
    return SharedPtr<T>::create(stack_allocator, std::forward<Args>(args)...);
}

}