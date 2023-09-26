#include <CrossFire.hpp>
using namespace CrossFire;

auto main() -> i32
{
    auto &log = Logger::get_stdout();
    log.set_timestamp(true);
    log.set_name("App");
    log.debug("This is a debug message.");
    log.info("This is an info message.");
    log.warn("This is a warning message.");
    log.err("This is an error message.");
    log.flush();

    cf_assert(false, "This is a test assertion.");
    return EXIT_SUCCESS;
}