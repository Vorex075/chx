# Tests for CircularQueue
add_executable(test_circular_queue test_circular_queue.cpp)
target_include_directories(test_circular_queue PRIVATE
    ${CMAKE_SOURCE_DIR}/external/doctest
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_circular_queue PRIVATE chx)
add_test(NAME circular_queue COMMAND test_circular_queue)

# Tests for ThreadSafeCircularQueue
add_executable(test_thread_safe_circular_queue test_thread_safe_circular_queue.cpp)
target_include_directories(test_thread_safe_circular_queue PRIVATE
    ${CMAKE_SOURCE_DIR}/external/doctest
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(test_thread_safe_circular_queue PRIVATE chx)
add_test(NAME thread_safe_circular_queue COMMAND test_thread_safe_circular_queue)

# Tests for UnbufferedChannel
add_executable(test_unbuffered_channel test_unbuffered_channel.cpp)
target_include_directories(test_unbuffered_channel PRIVATE
    ${CMAKE_SOURCE_DIR}/external/doctest
    ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_unbuffered_channel PRIVATE chx)
add_test(NAME unbuffered_channel COMMAND test_unbuffered_channel)

