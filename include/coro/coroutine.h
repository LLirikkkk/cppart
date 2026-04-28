#pragma once

#include <coroutine>

namespace art::coro {

    class Coroutine {
    private:
        struct PromiseType {
            Coroutine get_return_object();

            auto initial_suspend() noexcept;

            auto final_suspend() noexcept;

            void return_void();

            void unhandled_exception();

        private:
            /*Not implemented*/
        };

    public:
        using promise_type = PromiseType;

        explicit Coroutine(std::coroutine_handle<promise_type> h);

        Coroutine();
        ~Coroutine();

        // Non-copyable
        Coroutine(const Coroutine&) = delete;
        Coroutine& operator=(const Coroutine&) = delete;

        // Moveable
        Coroutine(Coroutine&& other) noexcept = default;
        Coroutine& operator=(Coroutine&& other) noexcept = default;

    public:
        promise_type& promise();

        static Coroutine& current();

    private:
        /*Not implemented*/
    };

} // namespace art::coro
