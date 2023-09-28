#pragma once
#include "Utilities/Types.hpp"

namespace CrossFire
{

enum class AllocationError {
    OutOfMemory,
    ReallocFailed,
};

struct Allocator {
    virtual ~Allocator() = default;

    virtual auto allocate(usize size,
                          usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> = 0;
    virtual auto deallocate(Slice<u8> ptr) -> void = 0;
    virtual auto reallocate(Slice<u8> ptr, usize size,
                            usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> = 0;

    template <typename T, typename... Args>
    auto create(Args &&...args) -> Result<T *, AllocationError>
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
    auto alloc(usize count) -> Result<Slice<T>, AllocationError>
    {
        auto ptr = allocate(sizeof(T) * count, alignof(T));
        if (ptr.is_err())
            return ptr.unwrap_err();

        return Slice<T>(static_cast<T *>(ptr.unwrap().ptr), count);
    }

    template <typename T> auto dealloc(Slice<T> slice) -> void
    {
        deallocate(Slice<u8>(slice.ptr, sizeof(T) * slice.len));
    }
};

struct CAllocator final : public Allocator {
    CAllocator() = default;
    ~CAllocator() override = default;

    auto allocate(usize size, usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
    auto deallocate(Slice<u8> ptr) -> void override;
    auto reallocate(Slice<u8> ptr, usize size,
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

    auto allocate(usize size, usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
    auto deallocate(Slice<u8> ptr) -> void override;
    auto reallocate(Slice<u8> ptr, usize size,
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

    auto allocate(usize size, usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
    auto deallocate(Slice<u8> ptr) -> void override;
    auto reallocate(Slice<u8> ptr, usize size,
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
    ~DebugAllocator() override = default;

    auto allocate(usize size, usize alignment = alignof(std::max_align_t))
        -> Result<Slice<u8>, AllocationError> override;
    auto deallocate(Slice<u8> ptr) -> void override;
    auto reallocate(Slice<u8> ptr, usize size,
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

}