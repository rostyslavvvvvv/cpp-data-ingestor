cmake_minimum_required(VERSION 3.10)
project(DataIngestor VERSION 1.0.0 LANGUAGES CXX)

# Enforce strict C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable aggressive compiler optimizations for high-throughput processing
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

find_package(Threads REQUIRED)

add_executable(DataIngestor main.cpp)
target_link_libraries(DataIngestor PRIVATE Threads::Threads)