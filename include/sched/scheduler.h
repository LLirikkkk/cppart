#pragma once

#include "resumable.h"

namespace art::sched {

/// Interface for schedulers that keep tasks in the intrusive list.
class IntrusiveListScheduler {
public:
  /// <b>Effects</b>: Enqueues task for later execution.
  ///
  /// <b>Throws</b>: Nothing.
  ///
  /// <b>Note</b>:
  /// - Task must not be currently queued in any scheduler.
  /// - All tasks that were spawned must be eventually executed.
  virtual void spawn(Resumable<IntrusiveListScheduler>& task) noexcept = 0;

  virtual ~IntrusiveListScheduler() = default;
};

} // namespace art::sched
