cmake_minimum_required(VERSION 4.0)
project(Sentinel)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)


### Source & Header files ###
file(GLOB_RECURSE SOURCES "src/*.cpp")


### Paths & Executable ###
# Create the executable target
add_executable(Sentinel ${SOURCES})

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/lib)
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/lib)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

target_include_directories(Sentinel PRIVATE 
    include 
        lib/rapidjson/include
        lib/BS_thread_pool/include
        lib/SDL_shadercross/include
)


### Librarys ###
add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)
add_subdirectory(lib/SDL_image EXCLUDE_FROM_ALL)
add_subdirectory(lib/simpleini)

# Link Librarys
target_link_libraries(Sentinel PRIVATE 
                      SDL3::SDL3
                      SDL3_image::SDL3_image
                      SimpleIni::SimpleIni
                    )


### Compile options ###
# Set compile options
target_compile_options(Sentinel PRIVATE -Wall -Wextra)

set_target_properties(Sentinel
    PROPERTIES
        OUTPUT_NAME sentinel
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)