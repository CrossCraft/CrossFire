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

struct Allocator {
    virtual ~Allocator() = default;

    [[nodiscard]] virtual auto
    allocate(usize size, usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> = 0;
    virtual auto deallocate(Slice<u8> ptr) -> void = 0;
    [[nodiscard]] virtual auto
    reallocate(Slice<u8> ptr, usize size,
               usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> = 0;

    template <typename T, typename... Args>
    [[nodiscard]] auto create(Args &&...args) -> Result<T *, AllocationError>
    {
        auto ptr = allocate(sizeof(T), alignof(T));
        if (ptr.is_err())
            return ptr.unwrap_err();

        return new (ptr.unwrap().ptr) T(std::forward<Args>(args)...);
    }

    template <typename T> auto destroy(T *ptr) -> void
    {
        ptr->~T();
        deallocate(Slice<u8>((u8 *)ptr, sizeof(T)));
    }

    template <typename T>
    [[nodiscard]] auto alloc(usize count) -> Result<Slice<T>, AllocationError>
    {
        auto ptr = allocate(sizeof(T) * count, alignof(T));
        if (ptr.is_err())
            return ptr.unwrap_err();

        return Slice<T>((T *)(ptr.unwrap().ptr), count);
    }

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

    template <typename T> auto dealloc(Slice<T> slice) -> void
    {
        deallocate(Slice<u8>((u8 *)slice.ptr, sizeof(T) * slice.len));
    }
};

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
 * @brief Named after the Double Balanced Binary Search Tree (DBBST) data structure.
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

// Implement a Unique Pointer
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

template <typename T, typename... Args>
inline auto create_unique_stack(Args &&...args)
    -> Result<UniquePtr<T>, AllocationError>
{
    return UniquePtr<T>::create(stack_allocator, std::forward<Args>(args)...);
}

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

template <typename T, typename... Args>
inline auto create_shared_stack(Args &&...args)
    -> Result<SharedPtr<T>, AllocationError>
{
    return SharedPtr<T>::create(stack_allocator, std::forward<Args>(args)...);
}

}