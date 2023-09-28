#include <Utilities/Time.hpp>
#include <chrono>

namespace CrossFire
{

auto get_time_microseconds() -> u64
{
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()
    ).count();
}

}