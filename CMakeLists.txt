cmake_minimum_required(VERSION 3.13)

project(imp)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE not set, setting it to Release")
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

############################################################################
# Find Raylib

find_package(raylib REQUIRED)

if(raylib_FOUND)
    message(STATUS "Found existing Raylib in ${raylib_INCLUDE_DIRS}")
    include_directories(${raylib_INCLUDE_DIRS})
endif()

set(IMP_INCLUDE_DIR "src")

include_directories(${IMP_INCLUDE_DIR})

set(IMP_SRC
    "src/commands.c"
    "src/constructs.c"
    "src/imp.c"
    "src/utils.c"
    "src/backends/default_backend.c"
    "src/backends/dynamic_backend.c"
)

add_library(imp ${IMP_SRC})

set_target_properties(imp PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

target_link_libraries(imp raylib)

install(TARGETS imp
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY "${IMP_INCLUDE_DIR}/"
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*")
