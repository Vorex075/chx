find_package(doctest REQUIRED)

add_executable(test_circular_queue test_circular_queue.cpp)
target_link_libraries(test_circular_queue PRIVATE chx doctest::doctest)

add_test(NAME circular_queue COMMAND test_circular_queue)
