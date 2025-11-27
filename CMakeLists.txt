cmake_minimum_required(VERSION 4.0)
project(Sentinel)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)



### Source & Header files ###
file(GLOB_RECURSE SOURCES "src/*.cpp")



### CMakeignore ###
# Read CMakeignore file
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/CMakeignore" CMAKEIGNORE_contents)

# Convert contents to a list
string(REPLACE "\n" ";" IGNORE_FILES "${CMAKEIGNORE_contents}")

# Initialize an empty list for valid ignore files
set(CMAKEIGNORE_FILES)

# Filter valid file patterns from the ignore file
foreach(file IN LISTS IGNORE_FILES)
    # Remove comments and check for empty entries
    string(STRIP "${file}" file_cleaned)
    if(NOT file_cleaned STREQUAL "")
        # Append valid cleaned file patterns to the list
        list(APPEND CMAKEIGNORE_FILES "${file_cleaned}")
    endif()
endforeach()

message(STATUS "Files to ignore: ${CMAKEIGNORE_FILES}")#

# Remove unwanted files from the SOURCES list
foreach(pattern IN LISTS CMAKEIGNORE_FILES)
    # Trim whitespace around the pattern
    string(STRIP "${pattern}" trimmed_pattern)

    # Remove exact file matches
    string(REGEX REPLACE "^" "${CMAKE_CURRENT_SOURCE_DIR}/" full_path "${trimmed_pattern}")
    list(REMOVE_ITEM SOURCES ${full_path})

    # Remove files matching patterns
    foreach(file_to_remove IN LISTS SOURCES)
        # Check if the file matches the trimmed pattern
        if (file_to_remove MATCHES "${trimmed_pattern}$")
            list(REMOVE_ITEM SOURCES ${file_to_remove})
        endif()
    endforeach()
endforeach()



### Paths & Executable ##
# Create the executable target
add_executable(Sentinel ${SOURCES})

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bin/lib)
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/lib)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

target_include_directories(Sentinel PRIVATE include)



### Librarys ###
add_subdirectory(lib/SDL EXCLUDE_FROM_ALL)
add_subdirectory(lib/SDL_image EXCLUDE_FROM_ALL)

# Link Librarys
target_link_libraries(Sentinel PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)



### Compile options ###
# Set compile options
target_compile_options(Sentinel PRIVATE -Wall -Wextra)

set_target_properties(Sentinel
    PROPERTIES
        OUTPUT_NAME sentinel
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin"
)