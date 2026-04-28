#include "sched/thread-pool.h"

namespace art::sched {
ThreadPool::ThreadPool(std::size_t threads) {
  if (threads == 0) {
    throw std::invalid_argument("Number of threads must be positive");
  }
}
} // namespace art::sched
