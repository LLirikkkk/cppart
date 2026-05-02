#pragma once

#include "coro/coroutine.h"

#include <atomic>
#include <coroutine>

namespace art::sync {

class Mutex;

namespace detail {

class MutexLockAwaiter {
  public:
    MutexLockAwaiter() = default;

    explicit MutexLockAwaiter(Mutex& mutex) noexcept;

    bool await_ready() const noexcept;

    bool await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) noexcept;

    static void await_resume() noexcept;

  private:
    std::coroutine_handle<coro::Coroutine::promise_type> handle_ = nullptr;
    std::atomic<MutexLockAwaiter*> next_ = nullptr;

    Mutex* mutex_ = nullptr;

    friend class art::sync::Mutex;
};

} // namespace detail

class Mutex {
  public:
    bool try_lock() noexcept;

    detail::MutexLockAwaiter lock() noexcept;

    std::suspend_never unlock() noexcept;

  private:
    enum class State : std::uint8_t {
        UNLOCKED,
        LOCKED_NO_WAITERS,
        LOCKED_HAS_WAITERS
    };

    std::atomic<State> state_ = State::UNLOCKED;
    detail::MutexLockAwaiter sentinel_;
    std::atomic<detail::MutexLockAwaiter*> tail_ = &sentinel_;

    friend class detail::MutexLockAwaiter;
};

} // namespace art::sync
