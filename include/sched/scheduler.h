#pragma once

#include "resumable.h"

namespace art::sched {

/// Interface for schedulers that keep tasks in the intrusive list.
class IntrusiveListScheduler {
public:
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
  virtual void spawn(Resumable<IntrusiveListScheduler>& task) noexcept = 0;

  virtual ~IntrusiveListScheduler() = default;
};

} // namespace art::sched
