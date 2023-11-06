#pragma once

#include <chrono>
#include <thread>
#include <tracy/Tracy.hpp>

#if TRACY_ENABLE
/**
 * @brief Sleeps for 100 milliseconds to allow the profiler to attach.
 */
#define PROFILE_ATTACH \
    std::this_thread::sleep_for(std::chrono::milliseconds(100))
#else
#define PROFILE_ATTACH
#endif

/**
 * @brief Sets a custom message to the profiler.
 */
#define PROFILE_MESSAGE(message) TracyMessageL(message)

/**
 * @brief Sets a custom zone to the profiler -- the zone name is the function name.
 */
#define PROFILE_ZONE ZoneScopedN(__FUNCTION__)

/**
 * @brief Profile a frame start.
 */
#define PROFILE_START_FRAME(name) FrameMarkStart(name)

/**
 * @brief Profile a frame end.
 */
#define PROFILE_END_FRAME(name) FrameMarkEnd(name)