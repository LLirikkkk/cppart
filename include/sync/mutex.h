#pragma once

namespace art::sync {

    // Must preserve FIFO ordering
    class Mutex {
    public:
        bool try_lock() noexcept;

        auto lock() noexcept;

        auto unlock() noexcept;

    private:
        /* Not implemented */
    };

} // namespace ct::sync
