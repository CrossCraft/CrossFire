#include <CrossFire.hpp>
#include <string>
using namespace CrossFire;

auto main() -> i32
{
    Timer timer;
    auto &log = Logger::get_stdout();
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