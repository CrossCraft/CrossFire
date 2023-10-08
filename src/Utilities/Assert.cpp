#include <Utilities/Types.hpp>
#include <Utilities/Logger.hpp>

namespace CrossFire
{

auto cf_assert(bool condition, const char *message) -> void
{
    PROFILE_ZONE;
#ifndef NDEBUG
    if (condition)
        return;

    if (message)
        Logger::get_stderr().err(message);

    Logger::get_stderr().err("Assertion failed!");
    Logger::get_stderr().flush();

    abort();
#else
    (void)condition;
    (void)message;
#endif
}

}