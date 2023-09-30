#include <CrossFire.hpp>
using namespace CrossFire;

auto main() -> i32
{
    auto &log = Logger::get_stdout();
    log.info("Hello, World!");

    return EXIT_SUCCESS;
}