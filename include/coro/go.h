#pragma once

namespace art::coro {

    template <typename Scheduler, typename Routine>
    void go(Scheduler& scheduler, const Routine& routine);

    template <typename Routine>
    void go(const Routine& routine);

} // namespace art::coro
