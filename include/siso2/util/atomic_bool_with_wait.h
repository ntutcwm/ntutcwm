#ifndef ATOMIC_BOOL_WITH_WAIT_H
#define ATOMIC_BOOL_WITH_WAIT_H

#if __cplusplus >= 202002L // C++20 (and later) code

#define atomic_bool_with_wait std::atomic_flag

#else

#include <atomic>
#include <mutex>
#include <condition_variable>

class atomic_bool_with_wait : public std::atomic<bool>
{
public:
    using std::atomic<bool>::atomic;

    void wait(bool old) const noexcept
    {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&]{ return load() != old; });
    }

    void notify_one() noexcept
    {
        std::lock_guard<std::mutex> lock(m);
        cv.notify_one();
    }

    void notify_all() noexcept
    {
        std::lock_guard<std::mutex> lock(m);
        cv.notify_all();
    }

    atomic_bool_with_wait& operator=(bool value) noexcept
    {
        std::atomic<bool>::operator=(value); // use base class assignment
        return *this;
    }

    void clear() noexcept
    {
        std::atomic<bool>::operator=(false);
    }

    bool test_and_set() noexcept
    {
        auto value = load();
        std::atomic<bool>::operator=(true);
        return value;
    }

    bool test() noexcept
    {
        return load();
    }

private:
    mutable std::mutex m;
    mutable std::condition_variable cv;
};

#endif

#endif