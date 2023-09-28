#pragma once
#include "Types.hpp"

namespace CrossFire
{

auto get_time_microseconds() -> u64;

struct Timer
{
    u64 start_time = 0;
    u64 current_time = 0;
    u64 last_time = 0;

    Timer() { reset(); }

    inline auto update() -> void
    {
        current_time = get_time_microseconds();
    }

    inline auto elapsed() const -> f64
    {
        return static_cast<f64>(current_time - start_time) / 1000000.0;
    }

    inline auto delta() const -> f64
    {
        return static_cast<f64>(current_time - last_time) / 1000000.0;
    }

    inline auto reset() -> void
    {
        start_time = get_time_microseconds();
        current_time = start_time;
        last_time = start_time;
    }
};

}