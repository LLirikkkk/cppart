#pragma once

namespace art::sync {

    class Event {
    public:
        void emit() noexcept;

        bool emitted() const noexcept;

        auto wait() noexcept;

    private:
        /* Not implemented */
    };

} // namespace art::sync
