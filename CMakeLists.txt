cmake_minimum_required(VERSION 3.10)
project(meltdown-poc VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Enable more warnings
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif ()

# Include directory
include_directories(include)

# Static library: meltdown
add_library(meltdown STATIC
        src/fr.c
        src/meltdown.c
)

# Executable that links to the meltdown library
add_executable(meltdown-demo
        app/main.c
)

target_link_libraries(meltdown-demo PRIVATE meltdown)
