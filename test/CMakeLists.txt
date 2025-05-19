add_executable(test_circular_queue test_circular_queue.cpp)

target_include_directories(test_circular_queue PRIVATE
    ${CMAKE_SOURCE_DIR}/external/doctest
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(test_circular_queue PRIVATE chx)

add_test(NAME circular_queue COMMAND test_circular_queue)
