#pragma once

#include "coroutine.h"

namespace art::coro {

namespace detail {

class YieldAwaiter {
  public:
    static bool await_ready() noexcept;

    static void await_suspend(std::coroutine_handle<Coroutine::promise_type> handle) noexcept;

    static void await_resume() noexcept;
};

} // namespace detail

/**
 * @brief Suspends the current coroutine and passes the execution to the scheduler.
 * @return <code>YieldAwaiter</code>.
 */
detail::YieldAwaiter yield() noexcept;

} // namespace art::coro
