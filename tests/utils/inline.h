#pragma once

#include "sched/resumable.h"

namespace art::test {

class InlineScheduler {
  InlineScheduler() = default;

public:
  void spawn(art::sched::Resumable<InlineScheduler>& resumable) {
    resumable.resume(*this);
  }

  static InlineScheduler& instance() {
    static InlineScheduler sched;
    return sched;
  }
};

} // namespace ct_test
