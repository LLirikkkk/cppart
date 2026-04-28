#pragma once

namespace art::sched {

template <typename Scheduler>
class Resumable {
public:
  virtual void resume(Scheduler&) noexcept = 0;
  virtual ~Resumable() = default;
};

} // namespace art::sched
