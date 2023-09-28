#include <CrossFire.hpp>
#include <string>
using namespace CrossFire;

auto main() -> i32
{
    Timer timer;
    auto gpa = GPAllocator(1024 * 1024);
    auto dbg = DebugAllocator(gpa);
    Allocator &allocator = dbg;

    auto result = allocator.allocate(1024);
    if (result.is_err())
        return EXIT_FAILURE;

    auto ptr = result.unwrap();
    auto slice = Slice<u8>(ptr.ptr, ptr.len);
    for (usize i = 0; i < slice.len; i++)
        slice[i] = i;

    auto &log = Logger::get_stdout();

    log.info(("Allocated " + std::to_string(dbg.get_current_usage()) +
              " bytes of memory.")
                 .c_str());
    log.info(("Peak usage: " + std::to_string(dbg.get_peak_usage()) +
              " bytes of memory.")
                 .c_str());

    if (!dbg.detect_leaks()) {
        log.info("No memory leaks detected.");
    } else {
        log.err("Memory leaks detected.");
        log.err(("Allocated " + std::to_string(dbg.get_alloc_count()) +
                 " blocks of memory.")
                    .c_str());
        log.err(("Deallocated " + std::to_string(dbg.get_dealloc_count()) +
                 " blocks of memory.")
                    .c_str());
        log.err(("Allocated " + std::to_string(dbg.get_alloc_size()) +
                 " bytes of memory.")
                    .c_str());
        log.err(("Deallocated " + std::to_string(dbg.get_dealloc_size()) +
                 " bytes of memory.")
                    .c_str());
    }

    log.debug("This is a debug message.");
    log.info("This is an info message.");
    log.warn("This is a warning message.");
    log.err("This is an error message.");
    log.flush();

    timer.update();
    auto elapsed = timer.elapsed();
    log.info(("Time elapsed: " + std::to_string(elapsed)).c_str());

    return EXIT_SUCCESS;
}