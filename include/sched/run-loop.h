#pragma once

#include "boost/intrusive/list.hpp"
#include "resumable.h"
#include "scheduler.h"

#include <cstddef>

namespace art::sched {

/// Single-thread FIFO run loop. Stores tasks in an intrusive list and executes them on demand.
///
/// Thread-safety:
/// - All methods are not thread-safe and must not be called concurrently.
class RunLoop final : public IntrusiveListScheduler {
public:
  RunLoop() = default;

  ~RunLoop() override = default;

  RunLoop(const RunLoop&) = delete;

  RunLoop& operator=(const RunLoop&) = delete;

  RunLoop(RunLoop&&) = delete;

  RunLoop& operator=(RunLoop&&) = delete;

  /// Runs at most limit queued tasks.
  ///
  /// \param limit Maximum number of tasks to execute.
  ///
  /// \return Number of executed tasks.
  std::size_t run_at_most(std::size_t limit) noexcept;

  /// Runs one queued task if available.
  ///
  /// \return <code>true</code> if task was executed, <code>false</code> if the queue was empty.
  bool run_next() noexcept;

  /// Run tasks until queue is empty.
  ///
  /// \return Number of executed tasks.
  std::size_t run() noexcept;

  /// Enqueues task for later execution.
  ///
  /// Precondition:
  /// - task is not currently queued in any scheduler.
  ///
  /// Contract:
  /// - This method must not allocate.
  /// - All tasks that were spawned must be eventually executed.
  ///
  /// \param task Task to enqueue.
  void spawn(Resumable<IntrusiveListScheduler>& task) noexcept override;

  /// \return <code>true</code> if no tasks are queued.
  bool empty() const noexcept;

private:
  boost::intrusive::list<Resumable<IntrusiveListScheduler>> queue_;
};

} // namespace art::sched
