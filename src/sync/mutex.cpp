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
    if (mutex_->state_.exchange(Mutex::State::LOCKED_HAS_WAITERS) != Mutex::State::UNLOCKED) {
        return true;
    }

    if (prev != &mutex_->sentinel_) {
        return true;
    }

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
    auto expected = State::UNLOCKED;
    return state_.compare_exchange_strong(expected, State::LOCKED_NO_WAITERS);
}

detail::MutexLockAwaiter Mutex::lock() noexcept {
    return detail::MutexLockAwaiter(*this);
}

std::suspend_never Mutex::unlock() noexcept {
    if (auto expected_state = State::LOCKED_NO_WAITERS;
        state_.compare_exchange_strong(expected_state, State::UNLOCKED)) {
        return {};
    }

    auto* head = sentinel_.next_.load();
    if (head == nullptr) {
        do {
            head = sentinel_.next_.load();
        } while (head == nullptr);
    }

    sentinel_.next_.store(nullptr);
    state_ = State::LOCKED_NO_WAITERS;

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
    state_ = State::LOCKED_HAS_WAITERS;

    head->handle_.promise().reschedule();

    return {};
}

} // namespace art::sync
