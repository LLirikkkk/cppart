#pragma once

#include "coroutine.h"

namespace art::coro {

template <typename Scheduler, typename Routine>
void go(Scheduler& scheduler, const Routine& routine) {
  scheduler.spawn(routine().promise());
}

template <typename Routine>
void go(const Routine& routine) {
  const auto& curr_coro = Coroutine::current();
  curr_coro.get_execution_context().SpawnToScheduler(routine());
}

} // namespace art::coro
