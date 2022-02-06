set(ROOT_DIR ${CMAKE_SOURCE_DIR})

include_directories(${ROOT_DIR}/lib)

link_directories(${ROOT_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(forkServer forkServer.cpp)
target_link_libraries(forkServer NetLib)