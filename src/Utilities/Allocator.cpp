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
    auto new_ptr = aligned_realloc(ptr, size, alignment);
#endif

    if (!new_ptr)
        return AllocationError::ReallocFailed;

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
}

auto LinearAllocator::reallocate(void *ptr, CrossFire::usize size,
                                 CrossFire::usize alignment)
    -> Result<void *, AllocationError>
{
    // Do nothing
    return ptr;
}

}