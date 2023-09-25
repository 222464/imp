cmake_minimum_required(VERSION 3.13)

project(imp_raylib_backend)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE not set, setting it to Release")
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(IMP_RAYLIB_BACKEND_INCLUDE_DIR "src")

include_directories(${IMP_RAYLIB_BACKEND_INCLUDE_DIR})

set(IMP_RAYLIB_BACKEND_SRC
    "src/raylib_backend.c"
)

############################################################################
# Find Raylib

find_package(raylib REQUIRED)

if(raylib_FOUND)
    message(STATUS "Found existing Raylib in ${raylib_INCLUDE_DIRS}")
    include_directories(${raylib_INCLUDE_DIRS})
endif()


add_library(imp_raylib_backend SHARED ${IMP_RAYLIB_BACKEND_SRC})

target_link_libraries(imp_raylib_backend raylib)

set_target_properties(imp_raylib_backend PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
