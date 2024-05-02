cmake_minimum_required(VERSION 3.28)
project(Mandelbrot)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
file(GLOB SOURCES src/*.cpp)
add_executable(generator ${SOURCES})
