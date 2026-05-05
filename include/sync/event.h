#pragma once

#include "coro/coroutine.h"

#include <atomic>
#include <coroutine>

namespace art::sync {

/**
 * @brief Primitive of synchronization that allows coroutines to wait something. Cannot be used after emission.
 */
class Event {
  private:
    class Awaiter {
      public:
        Awaiter() = default;

        explicit Awaiter(Event& event) noexcept;

        bool await_ready() const noexcept;

        bool await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) noexcept;

        static void await_resume() noexcept;

      private:
        std::coroutine_handle<coro::Coroutine::promise_type> handle_ = nullptr;
        Event* event_ = nullptr;

        Awaiter* next_ = nullptr;

        friend class Event;
    };

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
    Awaiter wait() noexcept;

  private:
    static Awaiter emitted_;

    std::atomic<Awaiter*> head_ = nullptr;
};

} // namespace art::sync
