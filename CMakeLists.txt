cmake_minimum_required(VERSION 3.20)
project(wisk_evtol_hil LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tooling options (turn on as you like)
option(ENABLE_WARNINGS_AS_ERRORS "Treat warnings as errors" ON)
if (MSVC)
  add_compile_options(/W4)

if (ENABLE_WARNINGS_AS_ERRORS)
  add_compile_options(/WX)
  endif()
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if (ENABLE_WARNINGS_AS_ERRORS) 
    add_compile_options(-Werror)
  endif()
endif()

# Source tree (no sources included in this skeleton)
# add_library(core ...)
# add_executable(sim ...)
# target_link_libraries(sim PRIVATE core)

message(STATUS "Skeleton layout only. Add sources under src/, tests under tests/, etc.")

add_library(sched src/sched/FixedStep.cpp)
target_include_directories(sched PUBLIC src)

# our demo app
add_executable(sim src/main.cpp)
target_link_libraries(sim PRIVATE sched)
