cmake_minimum_required(VERSION 3.7.0)

project(alsa-cpp-wrapper-example VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(alsa_cpp_wrapper ../alsa_cpp_wrapper.cpp)

add_executable(alsa_cpp_wrapper_example.out
  alsa_cpp_wrapper_test.cpp
)

target_link_libraries(alsa_cpp_wrapper_example.out alsa_cpp_wrapper -lasound)
