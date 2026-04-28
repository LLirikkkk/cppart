#pragma once

#include "resumable.h"

namespace art::sched {

/**
 * @brief Interface for schedulers that keep tasks in the intrusive list.
 */
class IntrusiveListScheduler {
public:
  /**
   * @brief Enqueues task for later execution.
   * @param task Task to enqueue.
   * @note Task must not be currently queued in any scheduler.
   */
  virtual void spawn(Resumable<IntrusiveListScheduler>& task) noexcept = 0;

  virtual ~IntrusiveListScheduler() = default;
};

} // namespace art::sched
