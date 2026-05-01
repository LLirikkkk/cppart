add_executable(cppart_tests)

target_sources(
        cppart_tests

        PRIVATE
        coro/run-loop-test.cpp
        coro/thread-pool-test.cpp
        sched/run-loop-test.cpp
        sched/thread-pool-test.cpp
        sync/event-test.cpp
        sync/event-tp-test.cpp
        utils/alloc-count.cpp
        utils/wait-group.cpp

        PRIVATE
        FILE_SET HEADERS
        BASE_DIRS utils
        FILES
        utils/alloc-count.h
        utils/inline.h
        utils/lambda-resumable.h
        utils/wait-group.h
)

target_include_directories(
        cppart_tests

        PRIVATE
        utils
)

cppart_configure_compiler(cppart_tests)
cppart_set_compiler_warnings(cppart_tests)

find_package(GTest CONFIG REQUIRED)

target_link_libraries(
        cppart_tests

        PRIVATE
        cppart::cppart
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(cppart_tests)
