#pragma once

#include <boost/intrusive/list_hook.hpp>

namespace art::sched {

class IntrusiveListScheduler;

/// Interface for unit of work that can be resumed by a scheduler.
template <typename Scheduler>
class Resumable {
public:
  /// Executes one step of the task on the provided scheduler.
  ///
  /// \param scheduler Scheduler to execute one step of the task on.
  virtual void resume(Scheduler& scheduler) noexcept = 0;

  virtual ~Resumable() = default;
};

/// Interface for unit of work that can be resumed by an IntrusiveListScheduler.
///
/// Contract:
/// - Must remain alive while it is queued in the scheduler.
template <>
class Resumable<IntrusiveListScheduler>
    : public boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::normal_link>> {
public:
  /// Executes one step of the task on the provided scheduler.
  ///
  /// \param scheduler Scheduler to execute one step of the task on.
  virtual void resume(IntrusiveListScheduler& scheduler) noexcept = 0;

  virtual ~Resumable() = default;
};

} // namespace art::sched
