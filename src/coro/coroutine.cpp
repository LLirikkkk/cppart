#include "coro/coroutine.h"

#include <exception>
#include <utility>

namespace art::coro {

namespace detail {

CurrentCoroutineGuard::CurrentCoroutineGuard(Coroutine& coro) noexcept
    : prev_(Coroutine::curr_) {
    Coroutine::curr_ = &coro;
}

CurrentCoroutineGuard::~CurrentCoroutineGuard() {
    Coroutine::curr_ = prev_;
}

ExecutionContext::ExecutionContext(void* scheduler, Descriptors descriptors) noexcept
    : scheduler_(scheduler)
    , descriptors_(descriptors) {}

void ExecutionContext::SpawnToScheduler(Coroutine coro) const noexcept {
    descriptors_.spawn_to_scheduler_(scheduler_, std::move(coro));
}

} // namespace detail

Coroutine Coroutine::PromiseType::get_return_object() {
    return Coroutine(std::coroutine_handle<promise_type>::from_promise(*this));
}

std::suspend_always Coroutine::PromiseType::initial_suspend() noexcept {
    return {};
}

std::suspend_always Coroutine::PromiseType::final_suspend() noexcept {
    return {};
}

void Coroutine::PromiseType::return_void() noexcept {}

void Coroutine::PromiseType::unhandled_exception() noexcept {
    std::terminate();
}

void Coroutine::PromiseType::resume(sched::IntrusiveListScheduler& scheduler) noexcept {
    resume_impl(scheduler);
}

void Coroutine::PromiseType::yield() noexcept {
    yielded_ = true;
}

bool Coroutine::PromiseType::is_yielded() noexcept {
    bool res = yielded_;
    yielded_ = false;

    return res;
}

detail::ExecutionContext& Coroutine::PromiseType::get_execution_context() noexcept {
    return ctx_;
}

Coroutine::Coroutine(const std::coroutine_handle<promise_type> h)
    : handle_(h) {}

Coroutine::Coroutine(Coroutine&& other) noexcept
    : handle_(std::exchange(other.handle_, nullptr)) {}

Coroutine& Coroutine::operator=(Coroutine&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    handle_ = std::exchange(other.handle_, nullptr);

    return *this;
}

Coroutine::promise_type& Coroutine::promise() const noexcept {
    return handle_.promise();
}

Coroutine& Coroutine::current() noexcept {
    return *curr_;
}

} // namespace art::coro
