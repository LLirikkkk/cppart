#pragma once

#include "coro/coroutine.h"

#include <atomic>
#include <coroutine>

namespace art::sync {

/**
 * @brief Primitive of synchronization that allows serializing access to a shared resource.
 */
class Mutex {
  private:
    class Awaiter {
      public:
        Awaiter() = default;

        explicit Awaiter(Mutex& mutex) noexcept;

        bool await_ready() const noexcept;

        bool await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) noexcept;

        static void await_resume() noexcept;

      private:
        std::coroutine_handle<coro::Coroutine::promise_type> handle_ = nullptr;
        std::atomic<Awaiter*> next_ = nullptr;

        Mutex* mutex_ = nullptr;

        friend class Mutex;
    };

  public:
    /**
     * @brief Tries to acquire the mutex without suspending.
     * @return <code>true</code> if the coroutine acquired the mutex.
     */
    bool try_lock() noexcept;

    /**
     * @brief Acquires the mutex. If the mutex is already acquired by other coroutine, the current coroutine suspends
     * until it acquires the mutex.
     * @return <code>MutexLockAwaiter</code>.
     */
    Awaiter lock() noexcept;

    /**
     * @brief Releases the mutex. Must be called after lock().
     * @return <code>std::suspend_never</code>.
     */
    std::suspend_never unlock() noexcept;

  private:
    void remove_from_wait_list(Awaiter* awaiter) noexcept;

    // If lowest bit is 0, mutex is released. If lowest bit is 1, mutex is acquired. Other bits is number of waiters.
    std::atomic<std::size_t> state_ = 0;
    Awaiter sentinel_;
    std::atomic<Awaiter*> tail_ = &sentinel_;
};

} // namespace art::sync
