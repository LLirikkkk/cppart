#pragma once

#include "sched/resumable.h"
#include "sched/scheduler.h"

#include <coroutine>

namespace art::coro {

class Coroutine;

namespace detail {

class CurrentCoroutineGuard {
public:
  explicit CurrentCoroutineGuard(Coroutine& coro) noexcept;
  ~CurrentCoroutineGuard();

private:
  Coroutine* prev_ = nullptr;
};

struct Descriptors {
  using SpawnToSchedulerF = void (*)(void* scheduler, Coroutine coro) noexcept;

  SpawnToSchedulerF spawn_to_scheduler_ = nullptr;
};

class ExecutionContext {
public:
  ExecutionContext() = default;
  ExecutionContext(void* scheduler, Descriptors descriptors) noexcept;
  ~ExecutionContext() = default;

  void SpawnToScheduler(Coroutine coro) const noexcept;

private:
  void* scheduler_ = nullptr;
  Descriptors descriptors_;
};

} // namespace detail

class Coroutine {
private:
  struct PromiseType : public sched::Resumable<sched::IntrusiveListScheduler> {
    Coroutine get_return_object();

    static std::suspend_always initial_suspend() noexcept;

    static std::suspend_always final_suspend() noexcept;

    static void return_void() noexcept;

    static void unhandled_exception() noexcept;

    void resume(sched::IntrusiveListScheduler& scheduler) noexcept override;

    void request_reschedule() noexcept;

  private:
    bool check_reschedule_requested() noexcept;

    template <typename Scheduler>
    void set_execution_context(Scheduler& scheduler) noexcept {
      ctx_ = detail::ExecutionContext(
          &scheduler,
          {.spawn_to_scheduler_ = [](void* scheduler_ctx, Coroutine coro) noexcept {
            static_cast<Scheduler*>(scheduler_ctx)->spawn(coro.promise());
          }}
      );
    }

    detail::ExecutionContext& get_execution_context() noexcept;

    template <typename Scheduler>
    void resume_impl(Scheduler& scheduler) noexcept {
      const auto handle = std::coroutine_handle<promise_type>::from_promise(*this);

      Coroutine curr_coro(handle);
      detail::CurrentCoroutineGuard guard(curr_coro);

      set_execution_context(scheduler);

      handle.resume();
      if (handle.done()) {
        handle.destroy();
      } else if (check_reschedule_requested()) {
        scheduler.spawn(*this);
      }
    }

    detail::ExecutionContext ctx_;
    bool reschedule_requested_ = false;

    friend class Coroutine;

    template <typename Scheduler, typename Routine>
    friend void go(Scheduler&, const Routine&);

    template <typename Routine>
    friend void go(const Routine&);
  };

public:
  using promise_type = PromiseType;

  explicit Coroutine(std::coroutine_handle<promise_type> h);

  Coroutine() = default;
  ~Coroutine() = default;

  Coroutine(const Coroutine&) = delete;
  Coroutine& operator=(const Coroutine&) = delete;

  Coroutine(Coroutine&& other) noexcept;
  Coroutine& operator=(Coroutine&& other) noexcept;

  promise_type& promise() const noexcept;

  static Coroutine& current() noexcept;

private:
  std::coroutine_handle<promise_type> handle_ = nullptr;

  inline static thread_local Coroutine* curr_ = nullptr;

  friend class detail::CurrentCoroutineGuard;
};

} // namespace art::coro
