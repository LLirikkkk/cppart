#include "sync/event.h"

namespace art::sync {

Event::Awaiter Event::emitted_;

Event::Awaiter::Awaiter(Event& event) noexcept
    : event_(&event) {}

bool Event::Awaiter::await_ready() const noexcept {
    return event_->emitted();
}

bool Event::Awaiter::await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) noexcept {
    handle_ = handle;
    auto* head_old = event_->head_.load(std::memory_order_acquire);
    do {
        if (head_old == &Event::emitted_) {
            return false;
        }

        next_ = head_old;
    } while (
        !event_->head_.compare_exchange_weak(head_old, this, std::memory_order_release, std::memory_order_acquire)
    );

    return true;
}

void Event::Awaiter::await_resume() noexcept {}

void Event::emit() noexcept {
    const auto* head_old = head_.exchange(&emitted_, std::memory_order_acq_rel);
    while (head_old != &emitted_ && head_old != nullptr) {
        const auto* next = head_old->next_;
        head_old->handle_.promise().reschedule();
        head_old = next;
    }
}

bool Event::emitted() const noexcept {
    return head_.load(std::memory_order_acquire) == &emitted_;
}

Event::Awaiter Event::wait() noexcept {
    return Awaiter(*this);
}

} // namespace art::sync
