cmake_minimum_required(VERSION 3.8)
project(Sentinel)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

## Librarys ##
find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)

## Source & Header files ##
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")

# Create the executable target
add_executable(Sentinel ${SOURCES} ${HEADERS})

target_include_directories(Sentinel PRIVATE include)

## Libs ##
# SDL
add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)
# SDL image
add_subdirectory(lib/SDL_image EXCLUDE_FROM_ALL)

## Link Librarys
target_link_libraries(Sentinel PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

# Set compile options
target_compile_options(Sentinel PRIVATE -Wall -Wextra)

# set the output directory for built objects.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "bin/lib")