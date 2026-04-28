#include "sched/run-loop.h"

namespace art::sched {

std::size_t RunLoop::run_at_most(std::size_t limit) noexcept {
  std::size_t completed = 0;
  while (completed < limit) {
    if (!run_next()) {
      break;
    }

    ++completed;
  }

  return completed;
}

bool RunLoop::run_next() noexcept {
  if (empty()) {
    return false;
  }

  auto& task = queue_.front();
  queue_.pop_front();
  task.resume(*this);

  return true;
}

std::size_t RunLoop::run() noexcept {
  std::size_t completed = 0;
  while (run_next()) {
    ++completed;
  }

  return completed;
}

void RunLoop::spawn(Resumable<IntrusiveListScheduler>& task) noexcept {
  queue_.push_back(task);
}

bool RunLoop::empty() const noexcept {
  return queue_.empty();
}

} // namespace art::sched
