#pragma once

#include "coro/coroutine.h"

#include <atomic>
#include <coroutine>

namespace art::sync {

class Event;

namespace detail {

class EventAwaiter {
  public:
    EventAwaiter() = default;

    explicit EventAwaiter(Event& event) noexcept;

    bool await_ready() const noexcept;

    bool await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) noexcept;

    static void await_resume() noexcept;

  private:
    std::coroutine_handle<coro::Coroutine::promise_type> handle_ = nullptr;
    Event* event_ = nullptr;

    EventAwaiter* next_ = nullptr;

    friend class art::sync::Event;
};

} // namespace detail

/**
 * @brief Primitive of synchronization that allows coroutines to wait something. Cannot be used after emission.
 */
class Event {
  public:
    /**
     * @brief Reschedules all waiting coroutines. If called more than once has no effect.
     */
    void emit() noexcept;

    /**
     * @brief Checks if the event is emitted.
     * @return <code>true</code> if event was emitted.
     */
    bool emitted() const noexcept;

    /**
     * @brief If the event is not emitted, suspends the coroutine until emission.
     * @return <code>EventAwaiter</code>.
     */
    detail::EventAwaiter wait() noexcept;

  private:
    static detail::EventAwaiter emitted_;

    std::atomic<detail::EventAwaiter*> head_ = nullptr;

    friend class detail::EventAwaiter;
};

} // namespace art::sync
