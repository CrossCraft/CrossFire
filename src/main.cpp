#include <CrossFire.hpp>
#include <string>
using namespace CrossFire;

auto main() -> i32
{
    Timer timer;
    auto gpa = GPAllocator(1024 * 1024);
    auto dbg = DebugAllocator(gpa);
    Allocator &allocator = dbg;

    auto list = TailQueue<i32>(allocator);

    auto &log = Logger::get_stdout();
    log.info(("Allocated " + std::to_string(dbg.get_current_usage()) +
              " bytes of memory.")
                 .c_str());

    for (i32 i = 0; i < 10; i++)
        list.push(i).unwrap();

    log.info(("Peak usage: " + std::to_string(dbg.get_peak_usage()) +
              " bytes of memory.")
                 .c_str());
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