#pragma once
#include "Types.hpp"

namespace CrossFire
{

/**
 * @brief Get the current time in microseconds.
 * @return The current time in microseconds.
 */
auto get_time_microseconds() -> u64;

/**
 * @brief The Timer class is a simple timer.
 */
struct Timer {
    u64 start_time = 0;
    u64 current_time = 0;
    u64 last_time = 0;

    /**
     * @brief Construct a new Timer object
     */
    Timer()
    {
        reset();
    }

    /**
     * @brief Update the timer.
     */
    inline auto update() -> void
    {
        current_time = get_time_microseconds();
    }

    /**
     * @brief Get the elapsed time in seconds.
     * @return The elapsed time in seconds.
     */
    [[nodiscard]] inline auto elapsed() const -> f64
    {
        return static_cast<f64>(current_time - start_time) / 1000000.0;
    }

    /**
     * @brief Get the delta time in seconds.
     * @return The delta time in seconds.
     */
    [[nodiscard]] inline auto delta() const -> f64
    {
        return static_cast<f64>(current_time - last_time) / 1000000.0;
    }

    /**
     * @brief Reset the timer.
     */
    inline auto reset() -> void
    {
        start_time = get_time_microseconds();
        current_time = start_time;
        last_time = start_time;
    }
};

}