#pragma once
#include <atomic>

namespace CrossFire
{

template <typename T> class LockGuard {
public:
    explicit LockGuard(T &lock)
        : lock(lock)
    {
        lock.lock();
    }
    ~LockGuard()
    {
        lock.unlock();
    }

private:
    T &lock;
};

class SpinLock {
public:
    SpinLock() = default;
    ~SpinLock() = default;

    auto lock() -> void
    {
        while (flag.test_and_set(std::memory_order_acquire))
            ;
    }
    auto try_lock() -> bool
    {
        return !flag.test_and_set(std::memory_order_acquire);
    }
    auto unlock() -> void
    {
        flag.clear(std::memory_order_release);
    }

private:
    std::atomic_flag flag;
};

}