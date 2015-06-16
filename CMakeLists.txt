cmake_minimum_required(VERSION 2.8)
project(caf_nexus CXX)

include_directories(. ${LIBCAF_INCLUDE_DIRS})

file(GLOB CAF_NEXUS_HDRS "caf/nexus/*.hpp")

set(CAF_NEXUS_SRCS
    src/main.cpp)

add_executable(nexus ${CAF_NEXUS_SRCS} ${CAF_NEXUS_HDRS})

target_link_libraries(nexus
                      ${LD_FLAGS}
                      ${LIBCAF_LIBRARIES}
                      ${PTHREAD_LIBRARIES})

install(PROGRAMS ${EXECUTABLE_OUTPUT_PATH}/nexus DESTINATION bin)
