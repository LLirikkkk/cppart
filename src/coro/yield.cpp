#include "coro/yield.h"

namespace art::coro {

namespace detail {

bool YieldAwaiter::await_ready() noexcept {
    return false;
}

void YieldAwaiter::await_suspend(std::coroutine_handle<Coroutine::promise_type> handle) noexcept {
    handle.promise().yield();
}

void YieldAwaiter::await_resume() noexcept {}

} // namespace detail

detail::YieldAwaiter yield() noexcept {
    return {};
}

} // namespace art::coro
