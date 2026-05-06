#pragma once

#include "boost/circular_buffer.hpp"
#include "coro/coroutine.h"

#include <boost/intrusive/list.hpp>

#include <coroutine>
#include <cstddef>
#include <memory>
#include <mutex>

namespace art::sync {

/**
 * @brief Buffered MPMC channel.
 * @tparam T Type of values transmitted through the channel.
 */
template <typename T>
class BufferedChannel {
  private:
    class Awaiter;
    class SendAwaiter;
    class RecvAwaiter;

    struct State {
        explicit State(std::size_t capacity)
            : buff_(capacity) {}

        void add_send_waiter(SendAwaiter& waiter, std::unique_lock<std::mutex>& /*unused*/) noexcept {
            waiters_.push_back(waiter);
            type_of_waiters_ = TypeOfWaiters::SEND;
        }

        void add_recv_waiter(RecvAwaiter& waiter, std::unique_lock<std::mutex>& /*unused*/) noexcept {
            waiters_.push_back(waiter);
            type_of_waiters_ = TypeOfWaiters::RECV;
        }

        enum class TypeOfWaiters : std::uint8_t {
            NONE,
            SEND,
            RECV
        };

        std::mutex mtx_;
        boost::circular_buffer<T> buff_;
        boost::intrusive::list<Awaiter> waiters_;
        TypeOfWaiters type_of_waiters_ = TypeOfWaiters::NONE;
    };

    class Awaiter
        : public boost::intrusive::list_base_hook<boost::intrusive::link_mode<boost::intrusive::normal_link>> {};

    class SendAwaiter : public Awaiter {
      public:
        template <typename U>
            requires (std::is_same_v<std::decay_t<U>, T>)
        SendAwaiter(State* state, U&& value) noexcept(std::is_nothrow_constructible_v<T, U&&>)
            : state_(state)
            , value_(std::forward<U>(value)) {}

        static bool await_ready() noexcept {
            return false;
        }

        bool await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) {
            handle_ = handle;

            std::unique_lock lock(state_->mtx_);

            if (!state_->waiters_.empty() && state_->type_of_waiters_ == State::TypeOfWaiters::RECV) {
                auto& waiter = static_cast<RecvAwaiter&>(state_->waiters_.front());
                state_->waiters_.pop_front();

                lock.unlock();

                std::construct_at(std::addressof(waiter.value_), std::move(value_));
                waiter.handle_.promise().reschedule();

                return false;
            }

            if (state_->buff_.size() < state_->buff_.capacity()) {
                state_->buff_.push_back(std::move(value_));

                return false;
            }

            state_->add_send_waiter(*this, lock);

            return true;
        }

        static void await_resume() noexcept {}

      private:
        std::coroutine_handle<coro::Coroutine::promise_type> handle_ = nullptr;
        State* state_ = nullptr;
        T value_;

        friend class RecvAwaiter;
    };

    class RecvAwaiter : public Awaiter {
      public:
        explicit RecvAwaiter(State* state) noexcept
            : state_(state) {}

        static bool await_ready() noexcept {
            return false;
        }

        bool await_suspend(std::coroutine_handle<coro::Coroutine::promise_type> handle) {
            handle_ = handle;

            std::unique_lock lock(state_->mtx_);

            if (!state_->buff_.empty()) {
                std::construct_at(std::addressof(value_), std::move(state_->buff_.front()));
                state_->buff_.pop_front();

                if (!state_->waiters_.empty()) {
                    auto& waiter = static_cast<SendAwaiter&>(state_->waiters_.front());
                    state_->waiters_.pop_front();

                    state_->buff_.push_back(std::move(waiter.value_));

                    lock.unlock();

                    waiter.handle_.promise().reschedule();
                }

                return false;
            }

            if (!state_->waiters_.empty() && state_->type_of_waiters_ == State::TypeOfWaiters::SEND) {
                auto& waiter = static_cast<SendAwaiter&>(state_->waiters_.front());
                state_->waiters_.pop_front();

                lock.unlock();

                std::construct_at(std::addressof(value_), std::move(waiter.value_));

                waiter.handle_.promise().reschedule();

                return false;
            }

            state_->add_recv_waiter(*this, lock);

            return true;
        }

        T await_resume() {
            auto res = std::move(value_);
            std::destroy_at(std::addressof(value_));

            return res;
        }

      private:
        std::coroutine_handle<coro::Coroutine::promise_type> handle_ = nullptr;
        State* state_ = nullptr;

        union {
            T value_;
        };

        friend class SendAwaiter;
    };

  public:
    explicit BufferedChannel(const std::size_t capacity)
        : state_(std::make_shared<State>(capacity)) {}

    SendAwaiter send(T&& value) const noexcept(std::is_nothrow_move_constructible_v<T>) {
        return SendAwaiter(state_.get(), std::move(value));
    }

    SendAwaiter send(const T& value) const noexcept(std::is_nothrow_copy_constructible_v<T>) {
        return SendAwaiter(state_.get(), value);
    }

    RecvAwaiter recv() const noexcept {
        return RecvAwaiter(state_.get());
    }

  private:
    std::shared_ptr<State> state_ = nullptr;
};

} // namespace art::sync
