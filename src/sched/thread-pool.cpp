#include "sched/thread-pool.h"

namespace art::sched {

ThreadPool::ThreadPool(const std::size_t threads) {
  if (threads == 0) {
    throw std::invalid_argument("Number of threads must be positive");
  }

  num_of_workers_ = threads;
}

ThreadPool::~ThreadPool() {
  for (auto& worker : workers_) {
    worker.request_stop();
  }

  {
    std::unique_lock lock(mutex_);

    cv_.notify_all();
  }

  for (auto& worker : workers_) {
    worker.join();
  }

  queue_.clear();
}

void ThreadPool::run() {
  assert(workers_.empty());

  workers_.reserve(num_of_workers_);
  for (std::size_t i = 0; i < num_of_workers_; ++i) {
    workers_.emplace_back([this](const std::stop_token& st) {
      worker_loop(st);
    });
  }
}

void ThreadPool::spawn(Resumable<IntrusiveListScheduler>& task) noexcept {
  {
    std::unique_lock lock(mutex_);

    queue_.push_back(task);
  }

  cv_.notify_one();
}

void ThreadPool::worker_loop(const std::stop_token& st) noexcept {
  while (true) {
    Resumable<IntrusiveListScheduler>* task = nullptr;

    {
      std::unique_lock lock(mutex_);

      cv_.wait(lock, [this, &st] {
        return !queue_.empty() || st.stop_requested();
      });

      if (st.stop_requested()) {
        return;
      }

      task = &queue_.front();
      queue_.pop_front();
    }

    task->resume(*this);
  }
}

} // namespace art::sched
