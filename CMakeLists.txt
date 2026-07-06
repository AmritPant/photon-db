cmake_minimum_required(VERSION 3.13)
project(photon-db CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB HANDLER_FILES "lib/request-handler/*.cpp")

set(SERVER_CORE_FILES
    src/main.cpp
    src/aof.cpp
    src/config.cpp
    lib/RdbParser.cpp
    lib/request-router.cpp
    lib/resp-parser.cpp
    lib/store.cpp
)

# Combine your core server files and the individual command handlers
set(SOURCE_FILES 
    ${SERVER_CORE_FILES} 
    ${HANDLER_FILES}
)

add_executable(redis ${SOURCE_FILES})

# Include directory setups
target_include_directories(redis PRIVATE 
    include 
    src 
    lib
    lib/request-handler
    /usr/include
)

target_link_libraries(redis PRIVATE Threads::Threads)