set(ROOT_DIR ${CMAKE_SOURCE_DIR})

include_directories(${ROOT_DIR}/lib)

link_directories(${ROOT_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(threadServer threadServer.cpp)
target_link_libraries(threadServer NetLib)

add_executable(threadPoolServer threadPoolServer.cpp)
target_link_libraries(threadPoolServer NetLib)