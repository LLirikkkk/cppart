#pragma once

#include <cstddef>

namespace art::sync {

    // Buffered MPMC Channel
    template <typename T>
    class BufferedChannel {
    public:
        explicit BufferedChannel(std::size_t capacity);

        auto send(T&& value);

        auto send(const T& value);

        auto recv();

    private:
        /* Not implemented */
    };

} // namespace art::sync
