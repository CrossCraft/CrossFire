#include <Utilities/Allocator.hpp>
#include <cstdlib>

namespace CrossFire
{

auto CAllocator::allocate(usize size, usize alignment)
    -> Result<void *, AllocationError>
{
    // MSVC doesn't support aligned_alloc
#if defined(_MSC_VER)
    auto ptr = _aligned_malloc(size, alignment);
#else // Aligned alloc is standard in C11
    auto ptr = aligned_alloc(alignment, size);
#endif

    if (!ptr)
        return AllocationError::OutOfMemory;

    return ptr;
}

auto CAllocator::deallocate(void *ptr) -> void
{
    // MSVC doesn't support aligned_alloc
#if defined(_MSC_VER)
    _aligned_free(ptr);
#else // Aligned alloc is standard in C11
    free(ptr);
#endif
}

auto CAllocator::reallocate(void *ptr, usize size, usize alignment)
    -> Result<void *, AllocationError>
{
    // MSVC doesn't support aligned_alloc
#if defined(_MSC_VER)
    auto new_ptr = _aligned_realloc(ptr, size, alignment);
#else // Aligned alloc is standard in C11
    auto new_ptr = aligned_alloc(size, alignment);
#endif

    if (!new_ptr)
        return AllocationError::ReallocFailed;

#if !defined(_MSC_VER)
    free(ptr);
#endif

    memcpy(new_ptr, ptr, size);

    return new_ptr;
}

LinearAllocator::LinearAllocator(usize size, Allocator &allocator)
    : backing_allocator(allocator)
{
    auto result = allocator.allocate(size);
    memory.ptr =
        static_cast<u8 *>(result.unwrap()); // Panics if allocation fails
    memory.len = size;
    offset = 0;
}

LinearAllocator::~LinearAllocator()
{
    backing_allocator.deallocate(memory.ptr);
    memory = {};
    offset = 0;
}

auto LinearAllocator::allocate(usize size, usize alignment)
    -> Result<void *, AllocationError>
{
    auto aligned_offset = (offset + alignment - 1) & ~(alignment - 1);
    if (aligned_offset + size > memory.len)
        return AllocationError::OutOfMemory;

    offset = aligned_offset + size;
    return memory.ptr + aligned_offset;
}
auto LinearAllocator::deallocate(void *ptr) -> void
{
    // Do nothing
    (void)ptr;
}

auto LinearAllocator::reallocate(void *ptr, CrossFire::usize size,
                                 CrossFire::usize alignment)
    -> Result<void *, AllocationError>
{
    (void)size;
    (void)alignment;

    // Do nothing
    return ptr;
}

StackAllocator::StackAllocator(usize size, Allocator &allocator)
    : backing_allocator(allocator)
{
    auto result = allocator.allocate(size);
    memory.ptr = static_cast<u8 *>(result.unwrap());
    memory.len = size;
    offset = 0;
    last_offset = 0;
}

StackAllocator::~StackAllocator()
{
    backing_allocator.deallocate(memory.ptr);
    memory = {};
    offset = 0;
    last_offset = 0;
}

auto StackAllocator::allocate(usize size, usize alignment)
    -> Result<void *, AllocationError>
{
    auto aligned_offset = (offset + alignment - 1) & ~(alignment - 1);
    if (aligned_offset + size > memory.len)
        return AllocationError::OutOfMemory;

    offset = aligned_offset + size;
    return memory.ptr + aligned_offset;
}
auto StackAllocator::deallocate(void *ptr) -> void
{
    if (ptr == memory.ptr + offset)
        offset = last_offset;
}

auto StackAllocator::reallocate(void *ptr, usize size, usize alignment)
    -> Result<void *, AllocationError>
{
    if (ptr == memory.ptr + offset) {
        auto aligned_offset = (last_offset + alignment - 1) & ~(alignment - 1);
        if (aligned_offset + size > memory.len)
            return AllocationError::OutOfMemory;

        offset = aligned_offset + size;
        return memory.ptr + aligned_offset;
    } else {
        return ptr;
    }
}

}