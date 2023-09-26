#include <Utilities/Types.hpp>
#include <Utilities/Logger.hpp>

namespace CrossFire
{

auto cf_assert(bool condition, const char *message) -> void
{
#ifdef NDEBUG
    if (condition)
        return;

    if (message)
        Logger::get_stderr().err(message);

    Logger::get_stderr().err("Assertion failed!");

    abort();
#endif
}

}