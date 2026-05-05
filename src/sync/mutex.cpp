#include "sync/mutex.h"

namespace art::sync {

Mutex::Awaiter::Awaiter(Mutex& mutex) noexcept
    : mutex_(&mutex) {}

bool Mutex::Awaiter::await_ready() const noexcept {
    return mutex_->try_lock();
}

bool Mutex::Awaiter::await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) noexcept {
    handle_ = handle;
    auto* prev = mutex_->tail_.exchange(this);
    prev->next_.store(this);

    mutex_->state_.fetch_add(2);
    if ((mutex_->state_.fetch_or(1) & 1) == 1) {
        return true;
    }

    mutex_->remove_from_wait_list(this);

    return false;
}

void Mutex::Awaiter::await_resume() noexcept {}

bool Mutex::try_lock() noexcept {
    std::uint64_t expected = 0;
    return state_.compare_exchange_strong(expected, 1);
}

Mutex::Awaiter Mutex::lock() noexcept {
    return Awaiter(*this);
}

std::suspend_never Mutex::unlock() noexcept {
    if (std::uint64_t expected_state = 1; state_.compare_exchange_strong(expected_state, 0)) {
        return {};
    }

    auto* head = sentinel_.next_.load();
    remove_from_wait_list(head);

    head->handle_.promise().reschedule();

    return {};
}

void Mutex::remove_from_wait_list(Awaiter* awaiter) noexcept {
    state_.fetch_sub(2);
    sentinel_.next_.store(nullptr);
    auto* next = awaiter->next_.load();
    if (next == nullptr) {
        if (auto* expected = awaiter; tail_.compare_exchange_strong(expected, &sentinel_)) {
            return;
        }

        do {
            next = awaiter->next_.load();
        } while (next == nullptr);
    }

    sentinel_.next_.store(next);
}

} // namespace art::sync
