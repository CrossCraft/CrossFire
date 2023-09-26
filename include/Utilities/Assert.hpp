#pragma once
#include "Logger.hpp"

namespace CrossFire
{

inline auto cf_assert(bool condition, const char *message = nullptr) -> void
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