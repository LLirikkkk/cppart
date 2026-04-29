#include "sched/thread-pool.h"

#include "coro/go.h"
#include "coro/yield.h"
#include "wait-group.h"

#include <gtest/gtest.h>

#include <cstddef>

namespace art::test {

using namespace art::sched;
using namespace art::coro;

class CoroutineThreadPoolTest : public ::testing::Test {
protected:
  CoroutineThreadPoolTest() {
    pool.run();
  }

  ThreadPool pool{4};
};

TEST_F(CoroutineThreadPoolTest, SingleYield) {
  const int YIELDS = 5;
  WaitGroup wg;

  wg.add(1);
  go(pool, [&](this auto) -> Coroutine {
    for (size_t i = 0; i < YIELDS; ++i) {
      co_await yield();
    }
    wg.done();
  });

  wg.wait();
}

TEST_F(CoroutineThreadPoolTest, MultipleYields) {
  const int YIELDS = 7;
  const int COROUTINES = 10;

  WaitGroup wg;

  for (size_t i = 0; i < COROUTINES; ++i) {
    wg.add(1);
    go(pool, [&](this auto) -> Coroutine {
      for (size_t j = 0; j < YIELDS; ++j) {
        co_await yield();
      }
      wg.done();
    });
  }
  wg.wait();
}

TEST_F(CoroutineThreadPoolTest, NestedYields) {
  WaitGroup wg;

  wg.add(1);
  go(pool, [&](this auto) -> Coroutine {
    for (size_t i = 0; i < 3; ++i) {
      co_await yield();
      for (size_t j = 0; j < 2; ++j) {
        co_await yield();
      }
    }
    wg.done();
  });

  wg.wait();
}

} // namespace art::test
