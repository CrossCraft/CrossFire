#pragma once
#include <atomic>

namespace CrossFire
{

/**
 * @brief A generic lock guard using RAII.
 * @tparam T The type of the lock.
 */
template <typename T> class LockGuard {
public:
    explicit LockGuard(T &lock)
        : lock(lock)
    {
        PROFILE_ZONE;
        lock.lock();
    }
    ~LockGuard()
    {
        PROFILE_ZONE;
        lock.unlock();
    }

private:
    T &lock;
};

/**
 * @brief A simple spinlock.
 */
class SpinLock {
public:
    SpinLock() = default;
    ~SpinLock() = default;

    auto lock() -> void
    {
        PROFILE_ZONE;
        while (flag.test_and_set(std::memory_order_acquire))
            ;
    }
    auto try_lock() -> bool
    {
        PROFILE_ZONE;
        return !flag.test_and_set(std::memory_order_acquire);
    }
    auto unlock() -> void
    {
        PROFILE_ZONE;
        flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag;
};

}