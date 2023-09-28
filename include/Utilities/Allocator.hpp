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
        -> Result<void *, AllocationError> = 0;
    virtual auto deallocate(void *ptr) -> void = 0;
    virtual auto reallocate(void *ptr, usize size,
                            usize alignment = alignof(std::max_align_t))
        -> Result<void *, AllocationError> = 0;

    template <typename T, typename... Args>
    auto create(Args &&...args) -> Result<T *, AllocationError>
    {
        auto ptr = allocate(sizeof(T), alignof(T));
        if (ptr.is_err())
            return ptr.unwrap_err();

        return new (ptr.unwrap()) T(std::forward<Args>(args)...);
    }

    template <typename T> auto destroy(T *ptr) -> void
    {
        ptr->~T();
        deallocate(ptr);
    }

    template <typename T>
    auto alloc(usize count) -> Result<Slice<T>, AllocationError>
    {
        auto ptr = allocate(sizeof(T) * count, alignof(T));
        if (ptr.is_err())
            return ptr.unwrap_err();

        return Slice<T>(static_cast<T *>(ptr.unwrap()), count);
    }

    template <typename T> auto dealloc(Slice<T> slice) -> void
    {
        deallocate(slice.ptr);
    }
};

struct CAllocator final : public Allocator {
    CAllocator() = default;
    ~CAllocator() override = default;

    auto allocate(usize size, usize alignment = alignof(std::max_align_t))
        -> Result<void *, AllocationError> override;
    auto deallocate(void *ptr) -> void override;
    auto reallocate(void *ptr, usize size,
                    usize alignment = alignof(std::max_align_t))
        -> Result<void *, AllocationError> override;
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
        -> Result<void *, AllocationError> override;
    auto deallocate(void *ptr) -> void override;
    auto reallocate(void *ptr, usize size,
                    usize alignment = alignof(std::max_align_t))
        -> Result<void *, AllocationError> override;
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
        -> Result<void *, AllocationError> override;
    auto deallocate(void *ptr) -> void override;
    auto reallocate(void *ptr, usize size,
                    usize alignment = alignof(std::max_align_t))
        -> Result<void *, AllocationError> override;
};

}