#include <CrossFire.hpp>
using namespace CrossFire;

auto main() -> i32
{
    auto &log = Logger::get_stdout();
    log.set_timestamp(true);
    log.set_name("App");
    log.info("Hello, world!\n");

    cf_assert(false, "This is a test assertion.");
    return EXIT_SUCCESS;
}