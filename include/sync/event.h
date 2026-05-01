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

class Event {
  public:
    void emit() noexcept;

    bool emitted() const noexcept;

    detail::EventAwaiter wait() noexcept;

  private:
    static detail::EventAwaiter emitted_;

    std::atomic<detail::EventAwaiter*> head_ = nullptr;

    friend class detail::EventAwaiter;
};

} // namespace art::sync
