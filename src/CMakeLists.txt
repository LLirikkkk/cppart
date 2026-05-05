add_library(cppart)
add_library(cppart::cppart ALIAS cppart)

target_compile_features(cppart PUBLIC cxx_std_23)
set_target_properties(cppart PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(
        cppart

        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_sources(
        cppart

        PRIVATE
        coro/coroutine.cpp
        coro/yield.cpp
        sched/run-loop.cpp
        sched/thread-pool.cpp
        sync/event.cpp
        sync/mutex.cpp

        PUBLIC
        FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
        FILES
        ${PROJECT_SOURCE_DIR}/include/coro/coroutine.h
        ${PROJECT_SOURCE_DIR}/include/coro/go.h
        ${PROJECT_SOURCE_DIR}/include/coro/yield.h
        ${PROJECT_SOURCE_DIR}/include/sched/resumable.h
        ${PROJECT_SOURCE_DIR}/include/sched/run-loop.h
        ${PROJECT_SOURCE_DIR}/include/sched/scheduler.h
        ${PROJECT_SOURCE_DIR}/include/sched/thread-pool.h
        ${PROJECT_SOURCE_DIR}/include/sync/channel.h
        ${PROJECT_SOURCE_DIR}/include/sync/event.h
        ${PROJECT_SOURCE_DIR}/include/sync/mutex.h
)

cppart_configure_compiler(cppart)
cppart_set_compiler_warnings(cppart)

find_package(boost_intrusive CONFIG REQUIRED)
find_package(boost_circular_buffer CONFIG REQUIRED)

target_link_libraries(
        cppart

        PUBLIC
        Boost::intrusive
        Boost::circular_buffer
)
