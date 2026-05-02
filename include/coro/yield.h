#pragma once

#include "coroutine.h"

namespace art::coro {

namespace detail {

/**
 * @brief Awaiter that suspends the current coroutine and passes the execution to the scheduler.
 */
class YieldAwaiter {
  public:
    static bool await_ready() noexcept;

    static void await_suspend(std::coroutine_handle<Coroutine::promise_type> handle) noexcept;

    static void await_resume() noexcept;
};

} // namespace detail

/**
 * @brief Creates a <code>YieldAwaiter</code>.
 * @return <code>YieldAwaiter</code> that suspends the current coroutine and passes the execution to the scheduler.
 */
detail::YieldAwaiter yield() noexcept;

} // namespace art::coro
