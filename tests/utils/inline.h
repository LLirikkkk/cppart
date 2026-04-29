#pragma once

#include "sched/resumable.h"

namespace art::test {

class InlineScheduler {
  InlineScheduler() = default;

public:
  void spawn(sched::Resumable<InlineScheduler>& resumable) {
    resumable.resume(*this);
  }

  static InlineScheduler& instance() {
    static InlineScheduler sched;
    return sched;
  }
};

} // namespace art::test
