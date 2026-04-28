#pragma once

#include "boost/intrusive/list.hpp"
#include "resumable.h"
#include "scheduler.h"

#include <cstddef>

namespace art::sched {

/**
 * @brief Single-thread FIFO run loop. Stores tasks in an intrusive list and executes them on demand.
 * @note All methods are not thread-safe and must not be called concurrently.
 */
class RunLoop final : public IntrusiveListScheduler {
public:
  RunLoop() = default;

  ~RunLoop() override = default;

  RunLoop(const RunLoop&) = delete;

  RunLoop& operator=(const RunLoop&) = delete;

  RunLoop(RunLoop&&) = delete;

  RunLoop& operator=(RunLoop&&) = delete;

  /**
   * @brief Runs at most <code>limit</code> queued tasks.
   * @param limit Number of maximum executed tasks.
   * @return Number of executed tasks.
   */
  std::size_t run_at_most(std::size_t limit) noexcept;

  bool run_next() noexcept;

  /**
   * @brief Runs one queued task if available.
   * @return <code>true</code> if task was executed, <code>false</code> if the queue was empty.
   */
  std::size_t run() noexcept;

  /**
   * @brief Enqueues task for later execution.
   * @param task Task to enqueue.
   * @note Task must not be currently queued in any scheduler.
   * @note All tasks that were spawned but not executed will be dropped without being resumed on destruction.
   */
  void spawn(Resumable<IntrusiveListScheduler>& task) noexcept override;

  /**
   * @brief checks if no tasks are queued.
   * @return <code>true</code> if no tasks are queued.
   */
  bool empty() const noexcept;

private:
  boost::intrusive::list<Resumable<IntrusiveListScheduler>> queue_;
};

} // namespace art::sched
