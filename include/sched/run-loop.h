#pragma once

#include "resumable.h"
#include "scheduler.h"

#include <cstddef>

namespace art::sched {

    class RunLoop final : public IntrusiveListScheduler {
    public:
        RunLoop();

        ~RunLoop();

        // Non-copyable
        RunLoop(const RunLoop&) = delete;
        RunLoop& operator=(const RunLoop&) = delete;

        // Non-moveable
        RunLoop(RunLoop&&) = delete;
        RunLoop& operator=(RunLoop&&) = delete;

        // Run at most `limit` tasks from queue
        // Returns number of completed tasks
        std::size_t run_at_most(std::size_t limit);

        // Run next task if queue is not empty
        // Returns true if task were completed
        bool run_next();

        // Run tasks until queue is empty
        // Returns number of completed tasks
        // Post-condition: empty() == true
        std::size_t run();

        void spawn(Resumable<IntrusiveListScheduler>& task) final;

        bool empty() const noexcept;

    private:
        /* Not implemented */
    };

} // namespace art::sched
