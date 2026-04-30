#pragma once

#include "coroutine.h"

namespace art::coro {

namespace detail {

struct YieldAwaiter {
  static bool await_ready() noexcept;

  static void await_suspend(std::coroutine_handle<Coroutine::promise_type> handle) noexcept;

  static void await_resume() noexcept;
};

} // namespace detail

detail::YieldAwaiter yield() noexcept;

} // namespace art::coro
