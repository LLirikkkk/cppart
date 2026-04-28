#pragma once

#include "resumable.h"

namespace art::sched {

    class IntrusiveListScheduler {
    public:
        // All tasks that were spawned must be eventually executed
        virtual void spawn(Resumable<IntrusiveListScheduler>&) = 0;
        virtual ~IntrusiveListScheduler() = default;
    };

} // namespace art::sched
