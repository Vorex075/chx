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

# Tests for BufferedChannel
add_executable(test_buffered_channel test_buffered_channel.cpp)
target_include_directories(test_buffered_channel PRIVATE
    ${CMAKE_SOURCE_DIR}/external/doctest
    ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_buffered_channel PRIVATE chx)
add_test(NAME buffered_channel COMMAND test_buffered_channel)

# Tests for SenderChannel
add_executable(test_sender_channel test_sender_channel.cpp)
target_include_directories(test_sender_channel PRIVATE
    ${CMAKE_SOURCE_DIR}/external/doctest
    ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_sender_channel PRIVATE chx)
add_test(NAME sender_channel COMMAND test_sender_channel)

# Tests for ReceiverChannel
add_executable(test_receiver_channel test_receiver_channel.cpp)
target_include_directories(test_receiver_channel PRIVATE
    ${CMAKE_SOURCE_DIR}/external/doctest
    ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_receiver_channel PRIVATE chx)
add_test(NAME receiver_channel COMMAND test_receiver_channel)
