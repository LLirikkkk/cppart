#pragma once

#include "resumable.h"
#include "scheduler.h"

#include <cstddef>

namespace art::sched {

    class ThreadPool final : public IntrusiveListScheduler {
    public:
        explicit ThreadPool(std::size_t threads);

        ~ThreadPool();

        // Non-copyable
        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        // Non-moveable
        ThreadPool(ThreadPool&&) = delete;
        ThreadPool& operator=(ThreadPool&&) = delete;

        void run();

        void spawn(Resumable<IntrusiveListScheduler>& task) final;

    private:
        /* Not implemented */
    };

} // namespace art::sched
