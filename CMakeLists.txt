# 1. Project Setup
# Set minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Define the project name and enable C and C++ languages
project(Mellodica LANGUAGES C CXX)

set(EXEC_NAME mellodica) # Use EXEC_NAME from Makefile for the target name

# 2. Optimization and Build Flags
# Map the Makefile's OPTIMIZATION_LEVEL to CMAKE_BUILD_TYPE.
# We'll set the default optimization level to -O1 unless specified.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (e.g., Debug, Release)")

# Apply optimization level and C++ standard
# The -O1 flag from the Makefile is often equivalent to setting CMAKE_BUILD_TYPE to 'Release'
# or explicitly adding flags based on the existing value.
# Since you used -O$(OPTIMIZATION_LEVEL), we set it explicitly for all configurations:

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 3. Find External Dependencies (SDL2, GLEW, GL, fluidsynth)
# CMake uses 'find_package' which is more robust than running shell commands.

# Find SDL2 and its components (Image, TTF)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Find OpenGL libraries
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

# Find fluidsynth
find_package(PkgConfig REQUIRED)
pkg_check_modules(FLUIDSYNTH REQUIRED fluidsynth)

# 4. Source Discovery and Include Directories
# Define directories
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# Recursively find all source files in the 'src' directory (similar to the 'find' command in Makefile)
file(GLOB_RECURSE SOURCE_FILES
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/*.c"
)

add_executable(${EXEC_NAME} ${SOURCE_FILES})

target_link_directories(${EXEC_NAME} PRIVATE ${FLUIDSYNTH_LIBRARY_DIRS})

# Add -Wall and -Wextra ONLY if the configuration is Debug.
target_compile_options(${EXEC_NAME}
    PUBLIC
    $<$<CONFIG:Debug>:-Wall -Wextra -Werror>
)

# 1. Define the base include directory
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# 2. Add ONLY the top-level 'include' folder to the target
target_include_directories(${EXEC_NAME}
    PUBLIC
    "${INCLUDE_DIR}" # <--- THIS IS THE FIX. No need to recursively list sub-directories.
    
    # Keep external library includes
    ${SDL2_INCLUDE_DIRS} 
    ${GLEW_INCLUDE_DIRS}
    ${FLUIDSYNTH_INCLUDE_DIRS}
)


# Link libraries and add include directories to the executable
target_link_libraries(${EXEC_NAME}
    # Link the found libraries (similar to LDFLAGS)
    SDL2::SDL2 # Replaces -lSDL2
    SDL2::SDL2main # Replaces -lSDL2main
    SDL2_image::SDL2_image # Replaces -lSDL2_image
    SDL2_ttf::SDL2_ttf # Replaces -lSDL2_ttf
    GLEW::GLEW # Replaces -lGLEW
    OpenGL::GL
    ${FLUIDSYNTH_LIBRARIES} # Replaces -lfluidsynth

)

# Add include directories (similar to INCLUDE_FLAGS)
target_include_directories(${EXEC_NAME}
    PUBLIC # PUBLIC ensures other targets linking to this one also get these includes
    "${INCLUDE_DIR}" # Add the base include directory
    ${INCLUDE_DIRS}  # Add all subdirectories recursively
    ${SDL2_INCLUDE_DIRS} # Add SDL's include path (replaces part of SDL_CFLAGS)
    ${GLEW_INCLUDE_DIRS}
    ${FLUIDSYNTH_INCLUDE_DIRS}
)

# 6. Clean and Run Targets
# CMake handles 'clean' automatically via 'cmake --build . --target clean'
# and manages object file placement (OBJ_DIR is no longer needed).

# Add a custom 'run' target (optional, but helpful for Makefile compatibility)
add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME}
    DEPENDS ${EXEC_NAME}
)


# Only enable packaging when we are building a Release build
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Packaging enabled for Release build")

    include(InstallRequiredSystemLibraries)

    # Install the executable
    install(TARGETS ${EXEC_NAME}
            BUNDLE DESTINATION .
            RUNTIME DESTINATION bin)

    set(CPACK_PACKAGE_FILE_NAME "Mellodica-${CMAKE_SYSTEM_NAME}")
    set(CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}")

    # ---- Linux AppImage ----
    if(UNIX AND NOT APPLE)
        set(CPACK_GENERATOR ZIP)
        set(CPACK_PROJECT_NAME ${PROJECT_NAME})
        #set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
    endif()

    # ---- macOS DMG ----
    if(APPLE)
        set(CPACK_GENERATOR DragNDrop)
        set(MACOSX_BUNDLE TRUE)
        set(CPACK_BUNDLE_NAME ${PROJECT_NAME})
        #set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
    endif()

    include(CPack)
endif()

