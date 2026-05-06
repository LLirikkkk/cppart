# cppart

<b>cppart</b> is a lightweight asynchronous runtime for C++23,
built on stackless coroutines. It provides a set of synchronization primitives 
and schedulers designed for high‑performance and low‑overhead coroutine execution.

# Features

- Stackless coroutines - uses C++23 coroutines for efficient, suspendable tasks.
- Coroutine helpers:
  - go(scheduler, callable) - spawns the coroutine on provided scheduler.
  - go(callable) - spawns the coroutine on the current scheduler (must be called from inside other coroutine).
  - yield() - suspends and immediately reschedule the current coroutine.
- Schedulers - all schedulers are divided to families depends on how they store tasks:
  - IntrusiveListScheduler:
    - RunLoop - single-threaded FIFO scheduler.
    - ThreadPool - fixed-size thread pool.
- Synchronization primitives:
  - Mutex - mutual exclusion with FIFO waiter queue.
  - Event - one-shot signal that resumes all waiters.
  - BufferedChannel< T > - MPMC channel with configurable capacity.
- Allocation-aware - minimal allocations.

# Requirements

- C++23 compiler.
- CMake 3.28 or later.
- Boost (Intrusive, Circular Buffer).
- gtest (for tests only).
