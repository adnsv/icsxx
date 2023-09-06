cmake_minimum_required(VERSION 3.20)

project(ics++)

add_library(ics++ INTERFACE)

target_compile_features(ics++ INTERFACE cxx_std_20)
target_include_directories(ics++ INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

option(ICS_TEST "Build ICS Tests" OFF)

if(ICS_TEST)
    add_subdirectory(test)
endif()