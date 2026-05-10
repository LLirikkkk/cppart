#include "sync/mutex.h"

namespace art::sync {

Mutex::Awaiter::Awaiter(Mutex& mutex) noexcept
    : mutex_(&mutex) {}

bool Mutex::Awaiter::await_ready() const noexcept {
    return mutex_->try_lock();
}

void Mutex::Awaiter::await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) noexcept {
    handle_ = handle;

    mutex_->state_.fetch_add(2, std::memory_order_relaxed);

    auto* prev = mutex_->tail_.exchange(this, std::memory_order_acq_rel);
    prev->next_.store(this, std::memory_order_release);
    if ((mutex_->state_.fetch_or(1, std::memory_order_acquire) & 1) == 1) {
        return;
    }

    mutex_->resume_next_waiter();
}

void Mutex::Awaiter::await_resume() noexcept {}

bool Mutex::try_lock() noexcept {
    std::uint64_t expected = 0;
    return state_.compare_exchange_strong(expected, 1, std::memory_order_acquire, std::memory_order_relaxed);
}

Mutex::Awaiter Mutex::lock() noexcept {
    return Awaiter(*this);
}

std::suspend_never Mutex::unlock() noexcept {
    if (std::uint64_t expected_state = 1;
        state_.compare_exchange_strong(expected_state, 0, std::memory_order_release, std::memory_order_relaxed)) {
        return {};
    }

    resume_next_waiter();

    return {};
}

void Mutex::resume_next_waiter() noexcept {
    auto* head = sentinel_.next_.load(std::memory_order_acquire);
    while (head == nullptr) {
        head = sentinel_.next_.load(std::memory_order_acquire);
    }

    remove_from_wait_list(head);

    head->handle_.promise().reschedule();
}

void Mutex::remove_from_wait_list(Awaiter* awaiter) noexcept {
    state_.fetch_sub(2, std::memory_order_relaxed);

    sentinel_.next_.store(nullptr, std::memory_order_relaxed);
    auto* next = awaiter->next_.load(std::memory_order_acquire);
    if (next == nullptr) {
        if (auto* expected = awaiter;
            tail_.compare_exchange_strong(expected, &sentinel_, std::memory_order_acq_rel, std::memory_order_relaxed)) {
            return;
        }

        do {
            next = awaiter->next_.load(std::memory_order_acquire);
        } while (next == nullptr);
    }

    sentinel_.next_.store(next, std::memory_order_release);
}

} // namespace art::sync
