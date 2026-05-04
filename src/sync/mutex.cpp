#include "sync/mutex.h"

namespace art::sync {

namespace detail {

MutexLockAwaiter::MutexLockAwaiter(Mutex& mutex) noexcept
    : mutex_(&mutex) {}

bool MutexLockAwaiter::await_ready() const noexcept {
    return mutex_->try_lock();
}

bool MutexLockAwaiter::await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) noexcept {
    handle_ = handle;
    auto* prev = mutex_->tail_.exchange(this);
    prev->next_.store(this);

    mutex_->state_.fetch_add(2);
    if ((mutex_->state_.fetch_or(1) & 1) == 1) {
        return true;
    }

    mutex_->state_.fetch_sub(2);

    mutex_->sentinel_.next_.store(nullptr);
    auto* next = next_.load();
    if (next == nullptr) {
        if (auto* expected = this; mutex_->tail_.compare_exchange_strong(expected, &mutex_->sentinel_)) {
            return false;
        }

        do {
            next = next_.load();
        } while (next == nullptr);
    }

    mutex_->sentinel_.next_.store(next);

    return false;
}

void MutexLockAwaiter::await_resume() noexcept {}

} // namespace detail

bool Mutex::try_lock() noexcept {
    std::uint64_t expected = 0;
    return state_.compare_exchange_strong(expected, 1);
}

detail::MutexLockAwaiter Mutex::lock() noexcept {
    return detail::MutexLockAwaiter(*this);
}

std::suspend_never Mutex::unlock() noexcept {
    if (std::uint64_t expected_state = 1; state_.compare_exchange_strong(expected_state, 0)) {
        return {};
    }

    state_.fetch_sub(2);

    auto* head = sentinel_.next_.load();
    sentinel_.next_.store(nullptr);
    auto* next = head->next_.load();
    if (next == nullptr) {
        if (auto* expected = head; tail_.compare_exchange_strong(expected, &sentinel_)) {
            head->handle_.promise().reschedule();

            return {};
        }

        do {
            next = head->next_.load();
        } while (next == nullptr);
    }

    sentinel_.next_.store(next);

    head->handle_.promise().reschedule();

    return {};
}

} // namespace art::sync
