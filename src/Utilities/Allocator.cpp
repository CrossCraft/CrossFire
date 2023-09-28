#include <Utilities/Allocator.hpp>
#include <cstdlib>

namespace CrossFire
{

auto CAllocator::allocate(usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError>
{
    if(size == 0)
        return AllocationError::InvalidSize;

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
    if(size == 0)
        return AllocationError::InvalidSize;

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
    if(size == 0)
        return AllocationError::InvalidSize;

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

GPAllocator::GPAllocator(usize size, Allocator &allocator) : free_map(), reserved_map(), backing_allocator(allocator)
{
    auto result = allocator.allocate(size);
    memory = result.unwrap();
    auto base = (usize)result.unwrap().ptr;

    free_map.emplace(base, Allocation(base, size));
}

GPAllocator::~GPAllocator() {
    backing_allocator.deallocate(memory);
    memory = {};

    free_map.clear();
    reserved_map.clear();
}

auto GPAllocator::allocate(usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError> {
    if(size == 0)
        return AllocationError::InvalidSize;

    // Calculate the aligned size
    auto aligned_size = (size + alignment - 1) & ~(alignment - 1);

    // Find a free block that is large enough
    for(auto it = free_map.begin(); it != free_map.end(); it++) {
        if(it->second.size >= aligned_size) {
            // Found a block that is large enough
            auto ret = Slice<u8>((u8 *)it->first, size);

            // Move the block from the free map to the reserved map
            auto block = it->second;
            free_map.erase(it);
            reserved_map.emplace(block.base, block);

            // Split the block if it is larger than the requested size
            if(block.size > aligned_size) {
                block.base += aligned_size;
                block.size -= aligned_size;

                // Insert the new block into the free map
                free_map.emplace(block.base, block);
            }

            // Return the allocated block
            return ret;
        }
    }

    // We couldn't find a free block that is large enough
    return AllocationError::OutOfMemory;
}
auto GPAllocator::deallocate(Slice<u8> ptr) -> void {
    auto base = (usize)ptr.ptr;

    // Find the block in the reserved map
    auto it = reserved_map.find(base);
    if(it == reserved_map.end())
        return; // The block was not found

    // Remove from the reserved map
    auto block = it->second;
    reserved_map.erase(it);

    // Check for adjacent free blocks
    auto freeIt = free_map.emplace(block.base, block);

    // Check to merge with the previous block
    for(auto predIt = free_map.begin(); predIt != free_map.end(); predIt++) {
        // Check if the block is before
        // And if the block is adjacent / overlapping
        if(predIt->second.base <= freeIt->second.base && predIt->second.base + predIt->second.size >= freeIt->second.base) {
            auto predBase = predIt->second.base;
            predIt->second.size += freeIt->second.size;
            free_map.erase(freeIt);

            // Update the iterator so we don't use an invalid iterator
            freeIt = free_map.find(predBase);
            break;
        }
    }

    // Check to merge with the next block
    for(auto succIt = free_map.begin(); succIt != free_map.end(); succIt++) {
        // Check if the block is after
        // And if the block is adjacent / overlapping
        if(freeIt->second.base <= succIt->second.base && freeIt->second.base + freeIt->second.size >= succIt->second.base) {
            freeIt->second.size += succIt->second.size;
            free_map.erase(succIt);
            break;
        }
    }

}
auto GPAllocator::reallocate(Slice<u8> ptr, usize size, usize alignment)
    -> Result<Slice<u8>, AllocationError> {
    auto result = allocate(size, alignment);

    if(result.is_err())
        return result.unwrap_err();

    memcpy(result.unwrap().ptr, ptr.ptr, ptr.len);

    deallocate(ptr);

    return result.unwrap();
}


}