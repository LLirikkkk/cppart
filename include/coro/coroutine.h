#pragma once

#include "sched/resumable.h"
#include "sched/scheduler.h"

#include <atomic>
#include <coroutine>

namespace art::coro {

class Coroutine;
struct PromiseType;

namespace detail {

/**
 * @brief RAII wrapper for currently running coroutine.
 */
class CurrentCoroutineGuard {
  public:
    explicit CurrentCoroutineGuard(Coroutine& coro) noexcept;
    ~CurrentCoroutineGuard();

  private:
    Coroutine* prev_ = nullptr;
};

/**
 * @brief Type-erased set of functions used by <code>ExecutionContext</code>.
 */
struct Descriptors {
    using SpawnToSchedulerF = void (*)(void* scheduler, PromiseType& promise) noexcept;

    SpawnToSchedulerF spawn_to_scheduler_ = nullptr;
};

/**
 * @brief Execution context of a running coroutine. Stores info about current execution.
 */
class ExecutionContext {
  public:
    ExecutionContext() = default;
    ExecutionContext(void* scheduler, Descriptors descriptors) noexcept;
    ~ExecutionContext() = default;

    /**
     * @brief Schedules the provided coroutine on the stored scheduler.
     * @param promise to schedule.
     * @pre The context must have been initialized.
     */
    void spawn_to_scheduler(PromiseType& promise) const noexcept;

  private:
    void* scheduler_ = nullptr;
    Descriptors descriptors_;
};

} // namespace detail

/**
 * @brief Is a lightweight handle used as a return type of coroutines and access the promise.
 * @note Intended usage is to create <code>Coroutine</code> and schedule it via <code>go(...)</code>.
 * @note Coroutine frame destroys on completion.
 */
class Coroutine {
  public:
    using promise_type = PromiseType;

    explicit Coroutine(std::coroutine_handle<promise_type> h);

    Coroutine() = default;
    ~Coroutine() = default;

    Coroutine(const Coroutine&) = delete;
    Coroutine& operator=(const Coroutine&) = delete;

    Coroutine(Coroutine&& other) noexcept;
    Coroutine& operator=(Coroutine&& other) noexcept;

    /**
     * @return The promise stored inside the coroutine frame.
     */
    promise_type& promise() const noexcept;

    /**
     * @return The currently running coroutine on this thread.
     * @note Calling this outside of coroutine execution is undefined behavior.
     */
    static Coroutine& current() noexcept;

  private:
    std::coroutine_handle<promise_type> handle_ = nullptr;

    inline static thread_local Coroutine* curr_ = nullptr;

    friend class detail::CurrentCoroutineGuard;
};

struct PromiseType : public sched::Resumable<sched::IntrusiveListScheduler> {
    Coroutine get_return_object();

    static std::suspend_always initial_suspend() noexcept;

    static std::suspend_always final_suspend() noexcept;

    static void return_void() noexcept;

    static void unhandled_exception() noexcept;

    /**
     * @brief Executes one step of the coroutine on the provided scheduler.
     * @param scheduler Scheduler which this task was executed on.
     */
    void resume(sched::IntrusiveListScheduler& scheduler) noexcept override;

    /**
     * @brief Requests yield.
     */
    void yield() noexcept;

    /**
     * @brief Reschedules suspended coroutine on the scheduler which it was executed on.
     */
    void reschedule() noexcept;

  private:
    /**
     * @brief Checks and resets the <code>yielded_</code>.
     * @return <code>true</code> if yield was requested.
     */
    bool is_yielded() noexcept;

    template <typename Scheduler>
    void set_execution_context(Scheduler& scheduler) noexcept {
        ctx_ = detail::ExecutionContext(
            &scheduler,
            {.spawn_to_scheduler_ = [](void* scheduler_ctx, PromiseType& promise) noexcept {
                static_cast<Scheduler*>(scheduler_ctx)->spawn(promise);
            }}
        );
    }

    detail::ExecutionContext& get_execution_context() noexcept;

    template <typename Scheduler>
    void resume_impl(Scheduler& scheduler) noexcept {
        const auto handle = std::coroutine_handle<PromiseType>::from_promise(*this);

        Coroutine curr_coro(handle);
        detail::CurrentCoroutineGuard guard(curr_coro);

        set_execution_context(scheduler);

        handle.resume();
        if (handle.done()) {
            handle.destroy();
        } else if (is_yielded()) {
            scheduler.spawn(*this);
        } else {
            reschedule();
        }
    }

    detail::ExecutionContext ctx_;
    bool yielded_ = false;
    std::atomic<bool> reschedule_requested_ = false;

    friend class Coroutine;

    template <typename Scheduler, typename Routine>
    friend void go(Scheduler&, const Routine&);

    template <typename Routine>
    friend void go(const Routine&);
};

} // namespace art::coro
