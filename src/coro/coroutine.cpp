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

void ExecutionContext::spawn_to_scheduler(PromiseType& promise) const noexcept {
    descriptors_.spawn_to_scheduler_(scheduler_, promise);
}

} // namespace detail

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

Coroutine PromiseType::get_return_object() {
    return Coroutine(std::coroutine_handle<PromiseType>::from_promise(*this));
}

std::suspend_always PromiseType::initial_suspend() noexcept {
    return {};
}

std::suspend_always PromiseType::final_suspend() noexcept {
    return {};
}

void PromiseType::return_void() noexcept {}

void PromiseType::unhandled_exception() noexcept {
    std::terminate();
}

void PromiseType::resume(sched::IntrusiveListScheduler& scheduler) noexcept {
    resume_impl(scheduler);
}

void PromiseType::yield() noexcept {
    yielded_ = true;
}

void PromiseType::reschedule() noexcept {
    if (!reschedule_requested_.exchange(true)) {
        return;
    }

    reschedule_requested_.store(false);

    ctx_.spawn_to_scheduler(*this);
}

bool PromiseType::is_yielded() noexcept {
    bool res = yielded_;
    yielded_ = false;

    return res;
}

detail::ExecutionContext& PromiseType::get_execution_context() noexcept {
    return ctx_;
}

} // namespace art::coro
