#pragma once

#include "boost/intrusive/list.hpp"
#include "resumable.h"
#include "scheduler.h"

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>

namespace art::sched {

/**
 * @brief Fixed sized thread pool scheduler.
 * @note All tasks that were spawned will be executed if run() was called.
 */
class ThreadPool final : public IntrusiveListScheduler {
public:
  /**
   * @brief Constructs thread pool with provided number of workers.
   * @param threads Number of workers.
   * @throws std::invalid_argument if threads = 0.
   */
  explicit ThreadPool(std::size_t threads);

  ~ThreadPool() override;

  ThreadPool(const ThreadPool&) = delete;

  ThreadPool& operator=(const ThreadPool&) = delete;

  ThreadPool(ThreadPool&&) = delete;

  ThreadPool& operator=(ThreadPool&&) = delete;

  /**
   * @brief Starts workers.
   * @throws std::logic_error If called more than once.
   * @note Must be called at most once.
   * @note Thread-safety: not thread-safe and must not be called concurrently.
   */
  void run();

  /**
   * @brief Enqueues task for later execution.
   * @param task Task to enqueue.
   * @note Task must not be currently queued in any scheduler.
   * @note Thread-safety: thread-safe.
   */
  void spawn(Resumable<IntrusiveListScheduler>& task) noexcept override;

private:
  void worker_loop(const std::stop_token& st) noexcept;

  std::size_t num_of_workers_ = 0;
  std::vector<std::jthread> workers_;

  std::mutex mutex_;
  boost::intrusive::list<Resumable<IntrusiveListScheduler>> queue_;

  std::condition_variable cv_;
};

} // namespace art::sched
