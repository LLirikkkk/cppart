# cppart

<b>cppart</b> is a lightweight asynchronous runtime for C++23,
built on stackless coroutines. It provides a set of synchronization primitives
and schedulers designed for high‑performance and low‑overhead coroutine execution.

## Features

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

## Requirements

- C++23 compiler.
- CMake 3.28 or later.
- Ninja (recommended for presets usage) or other CMake generator.
- Boost (Intrusive, Circular Buffer).
- gtest (for tests only).

## Build

Project uses CMake presets. You can build library or tests.

You can install dependencies via vcpkg (in that way you need to set CMAKE_TOOLCHAIN_FILE and VCPKG_ROOT variables in
Base preset in CMakePresets.json):

    vcpkg install

If you do not use vcpkg, install dependencies in your way.

Build library (Release):

    cmake --preset Release
    cmake --build --preset Release

File of built library locates in build/Release/src

Build tests (Release):

    cmake --preset Test-Release
    cmake --build --preset Test-Release

File of built tests locates in build/Test-Release/tests

All available presets:

Release, Debug, RelWithDebInfo, Sanitized, SanitizedDebug, ThreadSanitized, ThreadSanitizedDebug, Test-Release,
Test-Debug,
Test-RelWithDebInfo, Test-Sanitized, Test-SanitizedDebug, Test-ThreadSanitized, Test-ThreadSanitizedDebug.

## Run

You can run tests manually:

    ./build/Test-Release/tests/cppart_tests

Also, you can run tests via ctest:

    ctest --preset Test-Release
