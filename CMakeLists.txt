cmake_minimum_required(VERSION 4.0)
project(Sentinel)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

## Source & Header files ##
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h")

# Create the executable target
add_executable(Sentinel ${SOURCES} ${HEADERS})

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/lib)
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/lib)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

target_include_directories(Sentinel PRIVATE include)

## Librarys ##
# SDL
add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)
# SDL image
add_subdirectory(lib/SDL_image EXCLUDE_FROM_ALL)

## Link Librarys
target_link_libraries(Sentinel PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

# Set compile options
target_compile_options(Sentinel PRIVATE -Wall -Wextra)

set_target_properties(Sentinel
    PROPERTIES
        OUTPUT_NAME sentinel
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)