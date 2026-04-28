#pragma once

#include <boost/intrusive/list_hook.hpp>

namespace art::sched {

class IntrusiveListScheduler;

/**
 * @brief Interface for unit of work that can be resumed by a scheduler.
 * @tparam Scheduler Type of scheduler which this unit can be enqueued on.
 */
template <typename Scheduler>
class Resumable {
public:
  /**
   * @brief Executes one step of the task on the provided scheduler.
   * @param scheduler Scheduler which this task was executed on.
   */
  virtual void resume(Scheduler& scheduler) noexcept = 0;

  virtual ~Resumable() = default;
};

/**
 * @brief Interface for unit of work that can be resumed by an IntrusiveListScheduler.
 * @note Must remain alive while it is queued in the scheduler.
 */
template <>
class Resumable<IntrusiveListScheduler>
    : public boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::normal_link>> {
public:
  /**
   * @brief Executes one step of the task on the provided scheduler.
   * @param scheduler Scheduler which this task was executed on.
   */
  virtual void resume(IntrusiveListScheduler& scheduler) noexcept = 0;

  virtual ~Resumable() = default;
};

} // namespace art::sched
