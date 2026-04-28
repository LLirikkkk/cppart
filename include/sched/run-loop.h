#pragma once

#include "boost/intrusive/list.hpp"
#include "resumable.h"
#include "scheduler.h"

#include <cstddef>

namespace art::sched {

/// Single-thread FIFO run loop. Stores tasks in an intrusive list and executes them on demand.
///
/// <b>Note</b>: All methods are not thread-safe and must not be called concurrently.
class RunLoop final : public IntrusiveListScheduler {
public:
  RunLoop() = default;

  ~RunLoop() override = default;

  RunLoop(const RunLoop&) = delete;

  RunLoop& operator=(const RunLoop&) = delete;

  RunLoop(RunLoop&&) = delete;

  RunLoop& operator=(RunLoop&&) = delete;

  /// <b>Effects</b>: Runs at most limit queued tasks and returns number of executed tasks.
  ///
  /// <b>Throws</b>: Nothing.
  std::size_t run_at_most(std::size_t limit) noexcept;

  /// <b>Effects</b>: Runs one queued task if available and returns
  /// <code>true</code> if task was executed, <code>false</code> if the queue was empty.
  ///
  /// <b>Throws</b>: Nothing.
  bool run_next() noexcept;

  /// <b>Effects</b>: Run tasks until queue is empty and returns number of executed tasks.
  ///
  /// <b>Throws</b>: Nothing.
  std::size_t run() noexcept;

  /// <b>Effects</b>: Enqueues task for later execution.
  ///
  /// <b>Throws</b>: Nothing.
  ///
  /// <b>Note</b>:
  /// - Task must not be currently queued in any scheduler.
  /// - All tasks that were spawned but not executed will be dropped without being resumed on destruction.
  void spawn(Resumable<IntrusiveListScheduler>& task) noexcept override;

  /// <b>Effects</b>: returns <code>true</code> if no tasks are queued.
  ///
  /// <b>Throws</b>: Nothing.
  bool empty() const noexcept;

private:
  boost::intrusive::list<Resumable<IntrusiveListScheduler>> queue_;
};

} // namespace art::sched
