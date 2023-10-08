#include <CrossFire.hpp>
using namespace CrossFire;

auto main() -> i32
{
    PROFILE_ATTACH;
    PROFILE_ZONE;
    PROFILE_MESSAGE("Hello, World!");

    auto &log = Logger::get_stdout();

    for(auto i = 0; i < 1000; i++) {
        log.info("Hello, World!");
    }

    return EXIT_SUCCESS;
}