#include <Utilities/Allocator.hpp>
#include <cstdlib>

namespace CrossFire
{

auto CAllocator::allocate(usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError>
{
    // MSVC doesn't support aligned_alloc
#if defined(_MSC_VER)
    auto ptr = _aligned_malloc(size, alignment);
#else // Aligned alloc is standard in C11
    auto ptr = aligned_alloc(alignment, size);
#endif

    if (!ptr)
        return AllocationError::OutOfMemory;

    return Slice<u8>(static_cast<u8 *>(ptr), size);
}

auto CAllocator::deallocate(Slice<u8> ptr) -> void
{
    // MSVC doesn't support aligned_alloc
#if defined(_MSC_VER)
    _aligned_free(ptr.ptr);
#else // Aligned alloc is standard in C11
    free(ptr.ptr);
#endif
}

auto CAllocator::reallocate(Slice<u8> ptr, usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError>
{
    // MSVC doesn't support aligned_alloc
#if defined(_MSC_VER)
    auto new_ptr = _aligned_realloc(ptr.ptr, size, alignment);
#else // Aligned alloc is standard in C11
    auto new_ptr = aligned_alloc(size, alignment);
#endif

    if (!new_ptr)
        return AllocationError::ReallocFailed;

#if !defined(_MSC_VER)
    free(ptr.ptr);
#endif

    memcpy(new_ptr, ptr.ptr, size);

    return Slice<u8>(static_cast<u8 *>(new_ptr), size);
}

LinearAllocator::LinearAllocator(usize size, Allocator &allocator)
    : backing_allocator(allocator)
{
    auto result = allocator.allocate(size);
    memory = result.unwrap();
    offset = 0;
}

LinearAllocator::~LinearAllocator()
{
    backing_allocator.deallocate(memory);
    memory = {};
    offset = 0;
}

auto LinearAllocator::allocate(usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError>
{
    auto aligned_offset = (offset + alignment - 1) & ~(alignment - 1);
    if (aligned_offset + size > memory.len)
        return AllocationError::OutOfMemory;

    offset = aligned_offset + size;
    return Slice<u8>(memory.ptr + aligned_offset, size);
}
auto LinearAllocator::deallocate(Slice<u8> ptr) -> void
{
    // Do nothing
    (void)ptr;
}

auto LinearAllocator::reallocate(Slice<u8> ptr, CrossFire::usize size,
                                 CrossFire::usize alignment)
    -> Result<Slice<u8>, AllocationError>
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
    memory = result.unwrap();
    offset = 0;
    last_offset = 0;
}

StackAllocator::~StackAllocator()
{
    backing_allocator.deallocate(memory);
    memory = {};
    offset = 0;
    last_offset = 0;
}

auto StackAllocator::allocate(usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError>
{
    auto aligned_offset = (offset + alignment - 1) & ~(alignment - 1);
    if (aligned_offset + size > memory.len)
        return AllocationError::OutOfMemory;

    offset = aligned_offset + size;
    return Slice<u8>(memory.ptr + aligned_offset, size);
}
auto StackAllocator::deallocate(Slice<u8> ptr) -> void
{
    if (ptr.ptr == memory.ptr + offset)
        offset = last_offset;
}

auto StackAllocator::reallocate(Slice<u8> ptr, usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError>
{
    if (ptr.ptr == memory.ptr + offset) {
        auto aligned_offset = (last_offset + alignment - 1) & ~(alignment - 1);
        if (aligned_offset + size > memory.len)
            return AllocationError::OutOfMemory;

        offset = aligned_offset + size;
        return Slice<u8>(memory.ptr + aligned_offset, size);
    } else {
        return ptr;
    }
}

auto DebugAllocator::allocate(usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError>
{
    auto result = backing_allocator.allocate(size, alignment);
    if (result.is_err())
        return result.unwrap_err();

    alloc_count++;
    alloc_size += size;

    current_usage += size;
    if (current_usage > peak_usage)
        peak_usage = current_usage;

    for(usize i = 0; i < size; i++)
        result.unwrap().ptr[i] = 0xAA; // SET TO 0xAA TO DETECT UNINITIALIZED MEMORY

    return result.unwrap();
}
auto DebugAllocator::deallocate(Slice<u8> ptr) -> void
{
    for(usize i = 0; i < ptr.len; i++)
        ptr[i] = 0xDD; // SET TO 0xDD TO DETECT USE AFTER FREE

    backing_allocator.deallocate(ptr);

    dealloc_count++;
    dealloc_size += ptr.len;

    current_usage -= ptr.len;
}

auto DebugAllocator::reallocate(Slice<u8> ptr, usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError>
{
    auto result = backing_allocator.reallocate(ptr, size, alignment);
    if (result.is_err())
        return result.unwrap_err();

    alloc_size += size - ptr.len;
    current_usage += size - ptr.len;

    if (current_usage > peak_usage)
        peak_usage = current_usage;

    return result.unwrap();
}

CAllocator c_allocator = CAllocator();

}