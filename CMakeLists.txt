cmake_minimum_required(VERSION 3.16)

project(TextAdventureEngine)

add_executable(tae)

if (MSVC)
    target_compile_options(tae PRIVATE /Wall)
else()
    target_compile_options(tae PRIVATE -Wall -pedantic)
endif()

add_subdirectory(src)