#pragma once

#include "coroutine.h"

namespace art::coro {

/**
 * @brief Schedules coroutine on the provided scheduler.
 * @tparam Scheduler Type of scheduler which provided coroutine will be scheduled on.
 * @tparam Routine Type of callable returning <code>Coroutine</code>.
 * @param scheduler Scheduler which provided coroutine will be scheduled on.
 * @param routine Callable returning <code>Coroutine</code>.
 */
template <typename Scheduler, typename Routine>
void go(Scheduler& scheduler, const Routine& routine) {
  scheduler.spawn(routine().promise());
}

/**
 * @brief Schedules coroutine on the scheduler which current coroutine is running on.
 * @tparam Routine Type of callable returning <code>Coroutine</code>.
 * @param routine Callable returning <code>Coroutine</code>.
 * @note Must be called from within other coroutine.
 */
template <typename Routine>
void go(const Routine& routine) {
  const auto& curr_coro = Coroutine::current();
  curr_coro.promise().get_execution_context().SpawnToScheduler(routine());
}

} // namespace art::coro
