set(ROOT_DIR ${CMAKE_SOURCE_DIR})

include_directories(${ROOT_DIR}/lib)

link_directories(${ROOT_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(client client.cpp)
target_link_libraries(client NetLib)

add_executable(test test.cpp)
target_link_libraries(test NetLib)