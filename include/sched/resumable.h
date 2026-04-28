#pragma once

#include <boost/intrusive/list_hook.hpp>

namespace art::sched {

template <typename Scheduler>
class Resumable : public boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::normal_link>> {
public:
  virtual void resume(Scheduler&) noexcept = 0;

  virtual ~Resumable() = default;
};

} // namespace art::sched
