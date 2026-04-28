#pragma once

#include "boost/intrusive/list.hpp"
#include "resumable.h"
#include "scheduler.h"

#include <cstddef>

namespace art::sched {

class RunLoop final : public IntrusiveListScheduler {
public:
  RunLoop() = default;

  ~RunLoop() override = default;

  RunLoop(const RunLoop&) = delete;

  RunLoop& operator=(const RunLoop&) = delete;

  RunLoop(RunLoop&&) = delete;

  RunLoop& operator=(RunLoop&&) = delete;

  // Run at most `limit` tasks from queue
  // Returns number of completed tasks
  std::size_t run_at_most(std::size_t limit) noexcept;

  // Run next task if queue is not empty
  // Returns true if task were completed
  bool run_next() noexcept;

  // Run tasks until queue is empty
  // Returns number of completed tasks
  // Post-condition: empty() == true
  std::size_t run() noexcept;

  void spawn(Resumable<IntrusiveListScheduler>& task) noexcept override;

  bool empty() const noexcept;

private:
  boost::intrusive::list<Resumable<IntrusiveListScheduler>> queue_;
};

} // namespace art::sched
